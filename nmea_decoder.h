#pragma once

#include <string>
#include <vector>
#include <functional>
#include "NmeaAisStructures.h"

struct NmeaHeaderInfo {
    std::string raw_sentence;
    std::string talker_id;
    std::string message_type;
};

class nmea_decoder {
public:
    // ��������� ������� ��� �������� �������� ������
    using HeaderParsedCallback = std::function<void(const NmeaHeaderInfo&)>;
    using StandardMessageCallback = std::function<void(const std::string& talker, const std::string& type, const std::vector<std::string>& fields)>;
    using AisStringDetectedCallback = std::function<void(const std::string& ais_payload)>;

    nmea_decoder() = default;
    ~nmea_decoder() = default;

    // ������ �����������
    nmea_decoder(const nmea_decoder&) = delete;
    nmea_decoder& operator=(const nmea_decoder&) = delete;

    // ����������� ��������
    void SetOnHeaderParsed(HeaderParsedCallback cb);
    void SetOnStandardMessage(StandardMessageCallback cb);
    void SetOnAisStringDetected(AisStringDetectedCallback cb);

    /**
     * @brief ������ ������ ������ NMEA 0183 (���������� �� ����� ��� ��������� �� NMEA-450 �����).
     * @param sentence ������, ������������ ������ � '$' ��� '!'
     */
    void ParseSentence(const std::string& sentence);

private:
    // ���������� ������� ��������� � ����������� ������
    bool ValidateChecksum(const std::string& sentence) const;
    std::vector<std::string> SplitString(const std::string& str, char delimiter) const;

    HeaderParsedCallback m_header_cb = nullptr;
    StandardMessageCallback m_standard_cb = nullptr;
    AisStringDetectedCallback m_ais_cb = nullptr;
};
