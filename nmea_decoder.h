#pragma once

#include <string>
#include <vector>
#include <functional>
#include "NmeaAisStructures.h"

class nmea_decoder {
public:
    // Сигнатуры событий для подписки верхнего уровня
    using HeaderParsedCallback = std::function<void(const NmeaHeaderInfo&)>;
    using StandardMessageCallback = std::function<void(const std::string& talker, const std::string& type, const std::vector<std::string>& fields)>;
    using AisStringDetectedCallback = std::function<void(const std::string& ais_payload)>;

    nmea_decoder() = default;
    ~nmea_decoder() = default;

    // Запрет копирования
    nmea_decoder(const nmea_decoder&) = delete;
    nmea_decoder& operator=(const nmea_decoder&) = delete;

    // Регистрация колбэков
    void SetOnHeaderParsed(HeaderParsedCallback cb);
    void SetOnStandardMessage(StandardMessageCallback cb);
    void SetOnAisStringDetected(AisStringDetectedCallback cb);

    /**
     * @brief Разбор чистой строки NMEA 0183 (полученной из файла или очищенной от NMEA-450 тегов).
     * @param sentence Строка, начинающаяся строго с '$' или '!'
     */
    void ParseSentence(const std::string& sentence);

private:
    // Внутренние утилиты валидации и токенизации текста
    bool ValidateChecksum(const std::string& sentence) const;
    std::vector<std::string> SplitString(const std::string& str, char delimiter) const;

    HeaderParsedCallback m_header_cb = nullptr;
    StandardMessageCallback m_standard_cb = nullptr;
    AisStringDetectedCallback m_ais_cb = nullptr;
};
