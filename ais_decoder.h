#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include "NmeaAisStructures.h"

class ais_decoder {
public:
    // Сигнатуры колбэков для подписки бизнес-логики (паттерн Наблюдатель)
    using PositionCallback = std::function<void(const AisPositionReport&)>;
    using StaticDataCallback = std::function<void(const AisStaticDataReport&)>;

    ais_decoder() = default;
    ~ais_decoder() = default;

    // Запрет копирования семантики (модуль должен быть уникальным)
    ais_decoder(const ais_decoder&) = delete;
    ais_decoder& operator=(const ais_decoder&) = delete;

    // Методы регистрации обработчиков событий
    void SetOnPositionReport(PositionCallback cb);
    void SetOnStaticDataReport(StaticDataCallback cb);

    /**
     * @brief Главная точка входа. Декодирует только чистую инкапсулированную полезную нагрузку AIS.
     * @param ais_payload Шестибитная ASCII строка (например, из 5-го поля NMEA-предложения)
     */
    void DecodePayload(const std::string& ais_payload);

private:
    // Побитовые и текстовые внутренние утилиты
    std::vector<uint8_t> ConvertNmeaToSixBit(const std::string& ais_payload) const;
    uint32_t FetchBits(const std::vector<uint8_t>& bit_stream, size_t start_bit, size_t num_bits) const;
    std::string DecodeAisText(const std::vector<uint8_t>& bit_stream, size_t start_bit, size_t num_chars) const;
    
    double DecodeAisLongitude(int32_t raw_lon) const;
    double DecodeAisLatitude(int32_t raw_lat) const;

    // Зарегистрированные пользовательские колбэки
    PositionCallback m_position_cb = nullptr;
    StaticDataCallback m_static_cb = nullptr;
};
