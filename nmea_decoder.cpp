#include "nmea_decoder.h"
#include <sstream>

void nmea_decoder::SetOnHeaderParsed(HeaderParsedCallback cb) { m_header_cb = cb; }
void nmea_decoder::SetOnStandardMessage(StandardMessageCallback cb) { m_standard_cb = cb; }
void nmea_decoder::SetOnAisStringDetected(AisStringDetectedCallback cb) { m_ais_cb = cb; }

void nmea_decoder::ParseSentence(const std::string& sentence) {
    // 1. Первичная валидация структуры и длины
    if (sentence.length() < 6) return;
    if (sentence[0] != '$' && sentence[0] != '!') return;

    // 2. Проверка аппаратной контрольной суммы (XOR)
    if (!ValidateChecksum(sentence)) return;

    // 3. Вырезаем метаданные заголовка (Talker ID и Message Type)
    NmeaHeaderInfo header;
    header.raw_sentence = sentence;
    header.talker_id = sentence.substr(1, 2);   // Например, "GP" или "AI"
    header.message_type = sentence.substr(3, 3); // Например, "GGA" или "VDM"

    // Эмиссия события разбора заголовка для логгеров или СУБД
    if (m_header_cb) {
        m_header_cb(header);
    }

    // 4. Отрезаем контрольную сумму в конце строки (все, что после '*' включая саму '*')
    size_t star_pos = sentence.find('*');
    std::string body_without_checksum = sentence.substr(0, star_pos);

    // 5. Токенизация: разбиваем строку по запятым на массив полей
    std::vector<std::string> fields = SplitString(body_without_checksum, ',');

    // 6. Маршрутизация на основе протокола
    if (sentence[0] == '!') {
        // Протокол инкапсулированных данных (Морской AIS: !AIVDM, !AIVDO)
        if ((header.message_type == "VDM" || header.message_type == "VDO") && fields.size() >= 7) {
            if (m_ais_cb) {
                // Передаем строго 5-е поле, содержащее 6-битную строку
                m_ais_cb(fields[5]);
            }
        }
    } else {
        // Стандартный текстовый протокол (NMEA 0183: $GPGGA, $GPRMC, $HEHDT)
        if (m_standard_cb) {
            // Передаем Talker ID, тип и массив полей (начиная с индекса 1, так как 0 — это заголовок)
            std::vector<std::string> data_fields(fields.begin() + 1, fields.end());
            m_standard_cb(header.talker_id, header.message_type, data_fields);
        }
    }
}

bool nmea_decoder::ValidateChecksum(const std::string& sentence) const {
    size_t star = sentence.find('*');
    if (star == std::string::npos || star + 3 > sentence.length()) return false;

    uint8_t checksum = 0;
    // Считаем XOR всех символов строго между '$'/'!' и '*'
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

std::vector<std::string> nmea_decoder::SplitString(const std::string& str, char delimiter) const {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    // Если строка заканчивалась разделителем (например, ",,"), добавляем пустой токен в конец
    if (!str.empty() && str.back() == delimiter) {
        tokens.push_back("");
    }
    return tokens;
}
