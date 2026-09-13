#pragma once
#include <string>
#include <cstdint>

struct NmeaReport {
    // Базовый навигационный и текстовый контур
    std::string msg;             // Сообщение NMEA
    std::string header;          // 5-буквенный заголовок с 1-м символом (например, "$GPRMC", "!AIVDM")
    std::string device_descr;    // Talker ID (текстовое описание из файла)
    std::string manuf_descr;     // Производитель (текстовое описание из файла)
    std::string msg_type_descr;  // Тип сообщения (текстовое описание из файла)
    std::string src;             // Источник сообщения (из NMEA-450 тега \s:)
    std::string timestamp;       // Временная метка сообщения (из NMEA-450 тега или полей времени)
    std::string object_id;       // Идентификатор объекта (MMSI для AIS, ID цели для радара, "Internal GNSS")
    double lat = 0.0;            // Широта (десятичные градусы, WGS-84)
    double lon = 0.0;            // Долгота (WGS-84) в десятичных градусах
    double speed = 0.0;          // Скорость (Speed Over Ground) в узлах
    double heading = 0.0;        // Направление (Course Over Ground / True Heading) в градусах
    
    // Параметры прикладного уровня AIS
    bool is_ais = false;         // Флаг обнаружения AIS-трафика
    std::string ais_msg;         // Сообщение AIS (6-битный ASCII блок из 5-го поля NMEA)
    uint32_t ais_msg_type = 0;   // Тип сообщения AIS (числовой код 1, 2, 3, 4, 5, 18, 19, 27 по ITU-R M.1371)
    // Рейсовые данные (заполняются только для AIS тип 5)
    std::string ais_ship_name;   // Название судна
    std::string ais_call_sign;   // Позывной судна
    std::string ais_dest;        // Порт назначения
    
    bool has_pos = false;        // Наличие координат
};
