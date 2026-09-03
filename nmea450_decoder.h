#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>

// Структура для Sentence Grouping (многострочные сообщения NMEA-450)
struct NmeaGroupAssembly {
    int total_lines = 0;
    std::chrono::steady_clock::time_point timestamp;
    std::map<int, std::string> lines; // Номер строки -> Тело NMEA
};

class nmea450_decoder {
public:
    // Сигналы для передачи очищенного текста в следующий модуль (nmea_decoder)
    using SentenceAssembledCallback = std::function<void(const std::string& clean_sentence, const std::string& source_id)>;

    nmea450_decoder() = default;
    ~nmea450_decoder() = default;

    // Запрет копирования
    nmea450_decoder(const nmea450_decoder&) = delete;
    nmea450_decoder& operator=(const nmea450_decoder&) = delete;

    // Регистрация колбэка
    void SetOnSentenceAssembled(SentenceAssembledCallback cb);

    /**
     * @brief Обработка блока данных, полученного из транспортного уровня.
     * @param payload Указатель на начало полезной нагрузки
     * @param len Размер полезной нагрузки
     */
    void ProcessPacket(const uint8_t* payload, size_t len);

    /**
     * @brief Метод очистки недособранных многострочных пакетов по таймауту.
     *        Вызывается внешним планировщиком (nmea_service).
     */
    void CleanupTimeouts();

private:
    void HandleTagBlock(const std::string& tag_block, const std::string& nmea_sentence);
    std::vector<std::string> SplitString(const std::string& str, char delimiter) const;

    SentenceAssembledCallback m_assembled_cb = nullptr;
    std::map<std::string, NmeaGroupAssembly> m_nmea_group_pool;
};
