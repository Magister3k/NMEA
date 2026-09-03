#include "nmea450_decoder.h"
#include <cstring>
#include <sstream>

void nmea450_decoder::SetOnSentenceAssembled(SentenceAssembledCallback cb) { m_assembled_cb = cb; }

void nmea450_decoder::ProcessPacket(const uint8_t* payload, size_t len) {
    if (len < 8) return;

    // 1. Проверяем обязательный сетевой префикс стандарта "UdPbC\0"
    if (std::memcmp(payload, "UdPbC\0", 6) != 0) {
        return; // Пакет не соответствует стандарту IEC 61162-450
    }

    std::string payload_str(reinterpret_cast<const char*>(payload + 6), len - 6);
    
    // 2. Изолируем TAG-блок метаданных, обрамленный косыми чертами \...\
    if (!payload_str.empty() && payload_str[0] == '\\') {
        size_t close_tag = payload_str.find('\\', 1);
        if (close_tag != std::string::npos) {
            std::string tag_block = payload_str.substr(1, close_tag - 1);
            std::string nmea_sentence = payload_str.substr(close_tag + 1);
            
            // Зачищаем концевые переводы строк
            while(!nmea_sentence.empty() && (nmea_sentence.back() == '\n' || nmea_sentence.back() == '\r')) {
                nmea_sentence.pop_back();
            }

            HandleTagBlock(tag_block, nmea_sentence);
        }
    }
}

void nmea450_decoder::HandleTagBlock(const std::string& tag_block, const std::string& nmea_sentence) {
    size_t star_pos = tag_block.find('*');
    if (star_pos == std::string::npos) return;

    // Извлекаем блок параметров (до чексуммы)
    std::string tags_data = tag_block.substr(0, star_pos);
    std::vector<std::string> tags = SplitString(tags_data, ',');

    std::string source_id = "N450_DEV";
    std::string group_tag = "";

    // Парсим параметры блока тегов
    for (const auto& tag : tags) {
        if (tag.rfind("s:", 0) == 0) {       // Идентификатор источника (Source)
            source_id = tag.substr(2);
        } else if (tag.rfind("g:", 0) == 0) { // Тег многострочной группировки
            group_tag = tag.substr(2);
        }
    }

    // Сценарий А: Строка одиночная (не фрагментированная)
    if (group_tag.empty()) {
        if (m_assembled_cb) {
            m_assembled_cb(nmea_sentence, source_id);
        }
    } 
    // Сценарий Б: Строка является частью многострочной цепочки (например, AIS тип 5)
    else {
        // Формат g: [текущая_строка]-[всего_строк]-[id_группы] (Например: 1-2-5678)
        std::vector<std::string> g_parts = SplitString(group_tag, '-');
        if (g_parts.size() != 3) return;

        int current_line = std::stoi(g_parts[0]);
        int total_lines = std::stoi(g_parts[1]);
        std::string internal_group_id = source_id + "_" + g_parts[2]; // Уникализируем по источнику

        auto& group = m_nmea_group_pool[internal_group_id];
        group.timestamp = std::chrono::steady_clock::now();
        group.total_lines = total_lines;
        group.lines[current_line] = nmea_sentence;

        // Если все фрагменты цепочки успешно дошли, склеиваем полезную нагрузку
        if (group.lines.size() == static_cast<size_t>(group.total_lines)) {
            std::string fully_assembled_msg = "";
            
            for (int i = 1; i <= group.total_lines; ++i) {
                std::string part = group.lines[i];
                if (i == 1) {
                    fully_assembled_msg = part; // Первый фрагмент сохраняет базовый заголовок (! или $)
                } else {
                    // Из последующих строк вырезаем тело данных (между первой запятой и '*')
                    size_t first_comma = part.find(',');
                    size_t star = part.find('*');
                    if (first_comma != std::string::npos && star != std::string::npos && star > first_comma) {
                        fully_assembled_msg.insert(fully_assembled_msg.find('*'), part.substr(first_comma, star - first_comma));
                    }
                }
            }
            
            m_nmea_group_pool.erase(internal_group_id); // Удаляем сессию сборки

            if (m_assembled_cb) {
                m_assembled_cb(fully_assembled_msg, source_id);
            }
        }
    }
}

void nmea450_decoder::CleanupTimeouts() {
    auto now = std::chrono::steady_clock::now();
    // По стандарту IEC 61162-450 таймаут на сборку многострочных пакетов составляет 3 секунды
    for (auto it = m_nmea_group_pool.begin(); it != m_nmea_group_pool.end();) {
        if (now - it->second.timestamp > std::chrono::seconds(3)) {
            it = m_nmea_group_pool.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<std::string> nmea450_decoder::SplitString(const std::string& str, char delimiter) const {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
