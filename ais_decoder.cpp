#include "ais_decoder.h"
#include <algorithm>

void ais_decoder::SetOnPositionReport(PositionCallback cb) { m_position_cb = cb; }
void ais_decoder::SetOnStaticDataReport(StaticDataCallback cb) { m_static_cb = cb; }

// Метод принимает от l7_text_base вектор полей и ссылку на сквозной отчет
void ais_decoder::DecodeAndFillReport(const std::vector<std::string>& fields, NmeaPositionReport& report) {
    if (fields.size() < 6) return;

    // Сохраняем исходное сообщение AIS (чистый 6-битный ASCII блок из 5-го поля)
    report.ais_raw_payload = fields[5]; 
    report.is_ais = true;

    std::vector<uint8_t> bit_stream = ConvertNmeaToSixBit(report.ais_raw_payload);
    if (bit_stream.empty()) return;

    size_t total_bits = bit_stream.size() * 6;
    if (total_bits < 38) return;

    // Извлекаем тип сообщения AIS (биты 0-5)
    report.ais_msg_type = FetchBits(bit_stream, 0, 6);
    r_set_object_id = std::to_string(FetchBits(bit_stream, 6, 24)); // MMSI судна становится object_id
    report.object_id = r_set_object_id;

    // Разбор динамических координат, скорости и направления (Типы 1, 2, 3)
    if ((report.ais_msg_type == 1 || report.ais_msg_type == 2 || report.ais_msg_type == 3) && total_bits >= 168) {
        report.navigational_status = FetchBits(bit_stream, 30, 4);
        
        // Извлекаем скорость SOG (шаг 0.1 узла)
        uint32_t raw_sog = FetchBits(bit_stream, 42, 8);
        report.speed_knots = (raw_sog == 1023) ? 0.0 : raw_sog / 10.0;
        
        int32_t raw_lon = static_cast<int32_t>(FetchBits(bit_stream, 61, 28));
        if (raw_lon & 0x08000000) raw_lon |= 0xF0000000;
        report.longitude = DecodeAisLongitude(raw_lon);

        int32_t raw_lat = static_cast<int32_t>(FetchBits(bit_stream, 89, 27));
        if (raw_lat & 0x04000000) raw_lat |= 0xF8000000;
        report.latitude = DecodeAisLatitude(raw_lat);

        // Извлекаем направление COG (шаг 0.1 градуса)
        uint32_t raw_cog = FetchBits(bit_stream, 116, 12);
        report.heading_degrees = (raw_cog == 3600) ? 0.0 : raw_cog / 10.0;

        report.has_position = true;
    }
    // Разбор рейсовых данных (Тип 5)
    else if (report.ais_msg_type == 5 && total_bits >= 420) {
        report.ais_vessel_name = DecodeAisText(bit_stream, 104, 20);
        report.ais_call_sign = DecodeAisText(bit_stream, 62, 7);
        report.ais_destination = DecodeAisText(bit_stream, 302, 20);
        report.has_position = false; // Сообщение статическое, координат в нем нет
    }
    // Разбор спутникового формата (Тип 27)
    else if (report.ais_msg_type == 27 && total_bits >= 96) {
        uint32_t raw_sog = FetchBits(bit_stream, 43, 6);
        report.speed_knots = (raw_sog >= 63) ? 0.0 : static_cast<double>(raw_sog);

        int32_t raw_lon = static_cast<int32_t>(FetchBits(bit_stream, 49, 18));
        if (raw_lon & 0x00020000) raw_lon |= 0xFFFC0000;
        report.longitude = (raw_lon == 0x1A838) ? 0.0 : static_cast<double>(raw_lon) / 60.0; // Спутниковое масштабирование

        int32_t raw_lat = static_cast<int32_t>(FetchBits(bit_stream, 67, 17));
        if (raw_lat & 0x00010000) raw_lat |= 0xFFFE0000;
        report.latitude = (raw_lat == 0xD548) ? 0.0 : static_cast<double>(raw_lat) / 60.0;

        uint32_t raw_cog = FetchBits(bit_stream, 84, 9);
        report.heading_degrees = (raw_cog >= 511) ? 0.0 : static_cast<double>(raw_cog);
        report.has_position = true;
    }
}

std::string ais_decoder::DecodeAisText(const std::vector<uint8_t>& bit_stream, size_t start_bit, size_t num_chars) const {
    std::string text = "";
    for (size_t i = 0; i < num_chars; ++i) {
        uint32_t six_bit_char = FetchBits(bit_stream, start_bit + (i * 6), 6);
        char ascii_char = ' ';
        if (six_bit_char > 0 && six_bit_char < 32) {
            ascii_char = static_cast<char>(six_bit_char + 64); 
        } else if (six_bit_char >= 32 && six_bit_char <= 63) {
            ascii_char = static_cast<char>(six_bit_char);      
        }
        if (ascii_char == '@') ascii_char = ' ';
        text += ascii_char;
    }
    while (!text.empty() && text.back() == ' ') text.pop_back();
    return text;
}

std::vector<uint8_t> ais_decoder::ConvertNmeaToSixBit(const std::string& ais_payload) const {
    std::vector<uint8_t> result;
    result.reserve(ais_payload.length());
    for (char c : ais_payload) {
        uint8_t val = static_cast<uint8_t>(c);
        if (val < 48 || val > 119 || (val > 87 && val < 96)) continue;
        val -= 48;
        if (val > 40) val -= 8;
        result.push_back(val);
    }
    return result;
}

uint32_t ais_decoder::FetchBits(const std::vector<uint8_t>& bit_stream, size_t start_bit, size_t num_bits) const {
    uint32_t result = 0;
    for (size_t i = 0; i < num_bits; ++i) {
        size_t current_bit = start_bit + i;
        size_t byte_idx = current_bit / 6;    
        size_t bit_shift = 5 - (current_bit % 6); 

        if (byte_idx >= bit_stream.size()) return 0;
        uint32_t bit = (bit_stream[byte_idx] >> bit_shift) & 0x01;
        result = (result << 1) | bit;
    }
    return result;
}

double ais_decoder::DecodeAisLongitude(int32_t raw_lon) const {
    if (raw_lon == 0x6791AC0) return 0.0; 
    return static_cast<double>(raw_lon) / 600000.0;
}

double ais_decoder::DecodeAisLatitude(int32_t raw_lat) const {
    if (raw_lat == 0x3412140) return 0.0; 
    return static_cast<double>(raw_lat) / 600000.0;
}
