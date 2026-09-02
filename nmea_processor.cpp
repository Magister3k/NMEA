#include "nmea_processor.h"
#include <cstring>
#include <algorithm>
#include <sstream>

constexpr auto TCP_SESSION_TIMEOUT = std::chrono::minutes(5);
constexpr auto NMEA_GROUP_TIMEOUT = std::chrono::seconds(3);

nmea_processor::nmea_processor() {}

void nmea_processor::ProcUdpDatagram(const uint8_t* udp_payload, size_t udp_len) {
    if (udp_len < 2) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    // Проверяем префикс NMEA-450 "UdPbC\0"
    if (udp_len > 6 && std::memcmp(udp_payload, "UdPbC\0", 6) == 0) {
        HandleNmea450Packet(udp_payload + 6, udp_len - 6);
    } else {
        // Обычный NMEA-0183, завернутый в UDP датаграмму (текстовый блок)
        std::string raw_text(reinterpret_cast<const char*>(udp_payload), udp_len);
        ParseRawTextStream(raw_text, "UDP_0183");
    }
}

void nmea_processor::ProcTcpSegment(uint32_t src_ip, uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
                                            const uint8_t* tcp_payload, size_t tcp_len) 
{
    if (tcp_len == 0) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    TcpSessionKey key{ src_ip, dst_ip, src_port, dst_port };
    auto& session = m_tcp_pool[key];
    session.last_activity = std::chrono::steady_clock::now();

    // Дописываем пришедшие байты в конец скользящего буфера сессии
    session.data_accum.append(reinterpret_cast<const char*>(tcp_payload), tcp_len);

    // Формируем имя источника для логирования
    std::string source_id = "TCP_" + std::to_string(src_port);

    // Ищем маркеры конца строки (\n) в накопленном потоке
    size_t newline_pos;
    while ((newline_pos = session.data_accum.find('\n')) != std::string::npos) {
        // Вырезаем одну полную строку
        std::string line = session.data_accum.substr(0, newline_pos);
        // Удаляем её из буфера накопления
        session.data_accum.erase(0, newline_pos + 1);

        // Чистим \r
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // Если это строка NMEA-450 внутри TCP (редко, но бывает в логах)
        if (line.rfind("UdPbC\0", 0) == 0) {
            if (line.length() > 6) {
                HandleNmea450Packet(reinterpret_cast<const uint8_t*>(line.data() + 6), line.length() - 6);
            }
        } else if (!line.empty() && (line == '$' || line == '!')) {
            // Классический стриминг NMEA-0183
            if (ValidateNmeaChecksum(line)) {
                OnValidNmeaMsg(line, source_id);
            }
        }
    }

    // Защита от переполнения буфера (если гонят мусор без \n)
    if (session.data_accum.length() > 4096) {
        session.data_accum.clear(); 
    }
}

void nmea_processor::ParseRawTextStream(const std::string& text_stream, const std::string& source_id) {
    std::stringstream ss(text_stream);
    std::string sentence;
    while (std::getline(ss, sentence, '\n')) {
        if (!sentence.empty() && sentence.back() == '\r') {
            sentence.pop_back();
        }
        if (!sentence.empty() && (sentence == '$' || sentence == '!')) {
            if (ValidateNmeaChecksum(sentence)) {
                OnValidNmeaMsg(sentence, source_id);
            }
        }
    }
}

void nmea_processor::HandleNmea450Packet(const uint8_t* payload, size_t len) {
    std::string payload_str(reinterpret_cast<const char*>(payload), len);
    
    // Выделяем TAG-блок: \c:1672531199,s:GP001,g:1-2-1234*5F\$GPRMC...
    if (payload_str == '\\') {
        size_t close_tag = payload_str.find('\\', 1);
        if (close_tag != std::string::npos) {
            std::string tag_block = payload_str.substr(1, close_tag - 1);
            std::string nmea_sentence = payload_str.substr(close_tag + 1);
            
            while(!nmea_sentence.empty() && (nmea_sentence.back() == '\n' || nmea_sentence.back() == '\r')) {
                nmea_sentence.pop_back();
            }

            HandleNmea450TagBlock(tag_block, nmea_sentence);
        }
    }
}

void nmea_processor::HandleNmea450TagBlock(const std::string& tag_block, const std::string& nmea_sentence) {
    size_t star_pos = tag_block.find('*');
    if (star_pos == std::string::npos) return;

    std::string tags_data = tag_block.substr(0, star_pos);
    std::vector<std::string> tags = SplitString(tags_data, ',');

    std::string source_id = "N450_DEV";
    std::string group_tag = "";

    for (const auto& tag : tags) {
        if (tag.rfind("s:", 0) == 0) {
            source_id = tag.substr(2);
        } else if (tag.rfind("g:", 0) == 0) {
            group_tag = tag.substr(2);
        }
    }

    if (group_tag.empty()) {
        if (ValidateNmeaChecksum(nmea_sentence)) {
            OnValidNmeaMsg(nmea_sentence, source_id);
        }
    } else {
        // Сборка многострочной фрагментации (g: [current]-[total]-[group_id])
        std::vector<std::string> g_parts = SplitString(group_tag, '-');
        if (g_parts.size() != 3) return;

        int current_line = std::stoi(g_parts);
        int total_lines = std::stoi(g_parts);
        std::string internal_group_id = source_id + "_" + g_parts;

        auto& group = m_nmea_group_pool[internal_group_id];
        group.timestamp = std::chrono::steady_clock::now();
        group.total_lines = total_lines;
        group.lines[current_line] = nmea_sentence;

        if (group.lines.size() == static_cast<size_t>(group.total_lines)) {
            std::string fully_assembled_msg = "";
            for (int i = 1; i <= group.total_lines; ++i) {
                std::string part = group.lines[i];
                if (i == 1) {
                    fully_assembled_msg = part;
                } else {
                    size_t first_comma = part.find(',');
                    size_t star = part.find('*');
                    if (first_comma != std::string::npos && star != std::string::npos && star > first_comma) {
                        fully_assembled_msg.insert(fully_assembled_msg.find('*'), part.substr(first_comma, star - first_comma));
                    }
                }
            }
            m_nmea_group_pool.erase(internal_group_id);
            if (ValidateNmeaChecksum(fully_assembled_msg)) {
                OnValidNmeaMsg(fully_assembled_msg, source_id);
            }
        }
    }
}

void nmea_processor::TerminateTcpSession(uint32_t src_ip, uint32_t dst_ip, uint16_t src_port, uint16_t dst_port) {
    std::lock_guard<std::mutex> lock(m_mutex);
    TcpSessionKey key{ src_ip, dst_ip, src_port, dst_port };
    m_tcp_pool.erase(key);
}

bool nmea_processor::ValidateNmeaChecksum(const std::string& sentence) const {
    if (sentence.empty() || (sentence != '$' && sentence != '!')) return false;
    size_t star = sentence.find('*');
    if (star == std::string::npos || star + 3 > sentence.length()) return false;

    uint8_t checksum = 0;
    for (size_t i = 1; i < star; ++i) {
        checksum ^= static_cast<uint8_t>(sentence[i]);
    }

    std::string hex_str = sentence.substr(star + 1, 2);
    try {
        unsigned long target_checksum = std::stoul(hex_str, nullptr, 16);
        return checksum == static_cast<uint8_t>(target_checksum);
    } catch (...) {
        return false;
    }
}

void nmea_processor::CleanupTimeouts() {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto now = std::chrono::steady_clock::now();

    // Очистка зависших TCP сессий (если клиент отключился без FIN пакета)
    for (auto it = m_tcp_pool.begin(); it != m_tcp_pool.end();) {
        if (now - it->second.last_activity > TCP_SESSION_TIMEOUT) {
            it = m_tcp_pool.erase(it);
        } else {
            ++it;
        }
    }

    // Очистка недособранных многострочных NMEA предложений
    for (auto it = m_nmea_group_pool.begin(); it != m_nmea_group_pool.end();) {
        if (now - it->second.timestamp > NMEA_GROUP_TIMEOUT) {
            it = m_nmea_group_pool.erase(it);
        } else {
            ++it;
        }
    }
}

void nmea_processor::OnValidNmeaMsg(const std::string& nmea_sentence, const std::string& source_id) {
    std::cout << "[NMEA L4 PARSED] Src: " << source_id << " | Msg: " << nmea_sentence << std::endl;
}

std::vector<std::string> nmea_processor::SplitString(const std::string& str, char delimiter) const {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
