#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include "ais_structures.h"

class AisDecoder {
public:
    // ��������� �������� ��� �������� ������-������ (������� �����������)
    using PosCallback = std::function<void(const AisPosReport&)>;
    using DataCallback = std::function<void(const AisDataReport&)>;

    AisDecoder() = default;
    ~AisDecoder() = default;

    // ������ ����������� ��������� (������ ������ ���� ����������)
    AisDecoder(const AisDecoder&) = delete;
    AisDecoder& operator=(const AisDecoder&) = delete;

    // ������ ����������� ������������ �������
    void SetOnPosReport(PosCallback cb);
    void SetOnDataReport(DataCallback cb);

    /**
     * @brief ������� ����� �����. ���������� ������ ������ ����������������� �������� �������� AIS.
     * @param ais_payload 6-������ ASCII ������ (��������, �� 5-�� ���� NMEA-�����������)
     */
    void DecodePayload(const std::string& ais_payload);

private:
    // ��������� � ��������� ���������� �������
    std::vector<uint8_t> ConvertNmeaToSixBit(const std::string& ais_payload) const;
    uint32_t FetchBits(const std::vector<uint8_t>& bit_stream, size_t start_bit, size_t num_bits) const;
    std::string DecodeAisText(const std::vector<uint8_t>& bit_stream, size_t start_bit, size_t num_chars) const;
    
    double DecodeAisLon(int32_t raw_lon) const;
    double DecodeAisLat(int32_t raw_lat) const;

    // ������������������ ���������������� �������
    PosCallback m_pos_cb = nullptr;
    DataCallback m_data_cb = nullptr;
};
