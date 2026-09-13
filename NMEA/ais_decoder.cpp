#include "ais_decoder.h"
#include <algorithm>

void AisDecoder::SetOnPosReport(PosCallback cb) { m_pos_cb = cb; }
void AisDecoder::SetOnDataReport(DataCallback cb) { m_data_cb = cb; }

// ����� ��������� �� l7_text_base ������ ����� � ������ �� �������� �����
void AisDecoder::DecodeAndFillReport(const std::vector<std::string>& fields, NmeaReport& report) {
    if (fields.size() < 6) return;

    // ��������� �������� ��������� AIS (������ 6-������ ASCII ���� �� 5-�� ����)
    report.ais_msg = fields[5]; 
    report.is_ais = true;

    std::vector<uint8_t> bit_stream = ConvertNmeaToSixBit(report.ais_msg);
    if (bit_stream.empty()) return;

    size_t total_bits = bit_stream.size() * 6;
    if (total_bits < 38) return;

    // ��������� ��� ��������� AIS (���� 0-5)
    report.ais_msg_type = FetchBits(bit_stream, 0, 6);
    report.object_id = std::to_string(FetchBits(bit_stream, 6, 24)); // MMSI ����� ���������� object_id

    // ������ ������������ ���������, �������� � ����������� (���� 1, 2, 3)
    if ((report.ais_msg_type == 1 || report.ais_msg_type == 2 || report.ais_msg_type == 3) && total_bits >= 168) {
        report.nav_status = FetchBits(bit_stream, 30, 4);
        
        // ��������� �������� SOG (��� 0.1 ����)
        uint32_t raw_sog = FetchBits(bit_stream, 42, 8);
        report.speed = (raw_sog == 1023) ? 0.0 : raw_sog / 10.0;
        
        int32_t raw_lon = static_cast<int32_t>(FetchBits(bit_stream, 61, 28));
        if (raw_lon & 0x08000000) raw_lon |= 0xF0000000;
        report.lon = DecodeAisLon(raw_lon);

        int32_t raw_lat = static_cast<int32_t>(FetchBits(bit_stream, 89, 27));
        if (raw_lat & 0x04000000) raw_lat |= 0xF8000000;
        report.lat = DecodeAisLat(raw_lat);

        // ��������� ����������� COG (��� 0.1 �������)
        uint32_t raw_cog = FetchBits(bit_stream, 116, 12);
        report.heading = (raw_cog == 3600) ? 0.0 : raw_cog / 10.0;

        report.has_pos = true;
    }
    // ������ �������� ������ (��� 5)
    else if (report.ais_msg_type == 5 && total_bits >= 420) {
        report.ais_ship_name = DecodeAisText(bit_stream, 104, 20);
        report.ais_call_sign = DecodeAisText(bit_stream, 62, 7);
        report.ais_dest = DecodeAisText(bit_stream, 302, 20);
        report.has_pos = false; // ��������� �����������, ��������� � ��� ���
    }
    // ������ ������������ ������� (��� 27)
    else if (report.ais_msg_type == 27 && total_bits >= 96) {
        uint32_t raw_sog = FetchBits(bit_stream, 43, 6);
        report.speed = (raw_sog >= 63) ? 0.0 : static_cast<double>(raw_sog);

        int32_t raw_lon = static_cast<int32_t>(FetchBits(bit_stream, 49, 18));
        if (raw_lon & 0x00020000) raw_lon |= 0xFFFC0000;
        report.lon = (raw_lon == 0x1A838) ? 0.0 : static_cast<double>(raw_lon) / 60.0; // ����������� ���������������

        int32_t raw_lat = static_cast<int32_t>(FetchBits(bit_stream, 67, 17));
        if (raw_lat & 0x00010000) raw_lat |= 0xFFFE0000;
        report.lat = (raw_lat == 0xD548) ? 0.0 : static_cast<double>(raw_lat) / 60.0;

        uint32_t raw_cog = FetchBits(bit_stream, 84, 9);
        report.heading = (raw_cog >= 511) ? 0.0 : static_cast<double>(raw_cog);
        report.has_pos = true;
    }
}

std::string AisDecoder::DecodeAisText(const std::vector<uint8_t>& bit_stream, size_t start_bit, size_t num_chars) const {
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

std::vector<uint8_t> AisDecoder::ConvertNmeaToSixBit(const std::string& ais_payload) const {
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

uint32_t AisDecoder::FetchBits(const std::vector<uint8_t>& bit_stream, size_t start_bit, size_t num_bits) const {
    uint32_t result = 0;
    for (size_t i = 0; i < num_bits; ++i) {
        size_t cur_bit = start_bit + i;
        size_t byte_idx = cur_bit / 6;    
        size_t bit_shift = 5 - (cur_bit % 6); 

        if (byte_idx >= bit_stream.size()) return 0;
        uint32_t bit = (bit_stream[byte_idx] >> bit_shift) & 0x01;
        result = (result << 1) | bit;
    }
    return result;
}

double AisDecoder::DecodeAisLon(int32_t raw_lon) const {
    if (raw_lon == 0x6791AC0) return 0.0; 
    return static_cast<double>(raw_lon) / 600000.0;
}

double AisDecoder::DecodeAisLat(int32_t raw_lat) const {
    if (raw_lat == 0x3412140) return 0.0; 
    return static_cast<double>(raw_lat) / 600000.0;
}
