#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <chrono>

// Структура идентификации TCP-сессии (IP:Port -> IP:Port)
struct TcpSessionKey {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;

    bool operator==(const TcpSessionKey& other) const {
        return src_ip == other.src_ip && dst_ip == other.dst_ip && 
               src_port == other.src_port && dst_port == other.dst_port;
    }
};

namespace std {
    template <> struct hash<TcpSessionKey> {
        size_t operator()(const TcpSessionKey& k) const {
            return ((hash<uint32_t>()(k.src_ip) ^ (hash<uint32_t>()(k.dst_ip) << 1)) ^ 
                    (hash<uint16_t>()(k.src_port) << 2)) ^ hash<uint16_t>()(k.dst_port);
        }
    };
}

// Буфер для накопления потока TCP
struct TcpStreamBuffer {
    std::string data_accumulator;
    std::chrono::steady_clock::time_point last_activity;
};

// Структура для Sentence Grouping (многострочные сообщения NMEA-450)
struct NmeaGroupAssembly {
    int total_lines = 0;
    std::chrono::steady_clock::time_point timestamp;
    std::map<int, std::string> lines; // Номер строки -> Тело NMEA
};

class nmea_processor {
public:
    nmea_processor();
    virtual ~nmea_processor() = default;

    // Запрет копирования
    nmea_processor(const nmea_processor&) = delete;
    nmea_processor& operator=(const nmea_processor&) = delete;

    /**
     * @brief Обработка входящей монолитной UDP-датаграммы (после твоей IP-дефрагментации)
     * @param udp_payload Указатель на начало полезной нагрузки UDP (сразу после 8 байт заголовка UDP)
     * @param udp_len Размер полезной нагрузки UDP
     */
    void ProcUdpDatagram(const uint8_t* udp_payload, size_t udp_len);

    /**
     * @brief Обработка куска байтового потока TCP
     * @param tcp_payload Указатель на сегмент данных TCP
     * @param tcp_len Размер сегмента данных
     */
    void ProcTcpSegment(uint32_t src_ip, uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
                           const uint8_t* tcp_payload, size_t tcp_len);

    // Удаление сессии TCP при закрытии соединения (FIN/RST)
    void TerminateTcpSession(uint32_t src_ip, uint32_t dst_ip, uint16_t src_port, uint16_t dst_port);

    // Очистка мертвых буферов по таймауту
    void CleanupTimeouts();

protected:
    // Виртуальный колбэк для интеграции с твоей бизнес-логикой
    virtual void OnValidNmeaMsg(const std::string& nmea_sentence, const std::string& source_id);

private:
    std::mutex m_mutex;

    // Пулы сессий
    std::unordered_map<TcpSessionKey, TcpStreamBuffer> m_tcp_pool;
    std::unordered_map<std::string, NmeaGroupAssembly> m_nmea_group_pool;

    // Внутренний конвейер разбора строк
    void ParseRawTextStream(const std::string& text_stream, const std::string& source_id);
    void HandleNmea450Packet(const uint8_t* payload, size_t len);
    void HandleNmea450TagBlock(const std::string& tag_block, const std::string& nmea_sentence);
    
    // Валидация
    bool ValidateNmeaChecksum(const std::string& sentence) const;
    std::vector<std::string> SplitString(const std::string& str, char delimiter) const;
};
