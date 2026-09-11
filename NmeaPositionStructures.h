#pragma once
#include <string>
#include <cstdint>

struct NmeaPositionReport {
    // Базовый навигационный и текстовый контур
    std::string raw_sentence;       // 1. Сообщение NMEA
    std::string five_letter_head;   // 2. 5-буквенный заголовок с 1-м символом (например, "$GPRMC", "!AIVDM")
    std::string device_description; // 3. Talker ID (текст) ИЛИ Код производителя (текст) из файлов
    std::string message_type_desc;  // 4. Тип сообщения NMEA (текстовое описание) из файла
    std::string message_source;     // 5. Источник сообщения (канал/порт/тег s:)
    std::string timestamp;          // 6. Timestamp сообщения (из NMEA-450 тега или полей времени)
    std::string object_id;          // 7. Идентификатор объекта (MMSI для AIS, ID цели для радара, "Internal GNSS")
    double latitude = 0.0;          // 8. Широта (десятичные градусы WGS-84)
    double longitude = 0.0;         // 9. Долгота (десятичные градусы WGS-84)
    
    // Новые навигационные параметры (унифицированные для GPS/Радаров/AIS)
    double speed_knots = 0.0;       // 10. Скорость (Speed Over Ground, в узлах)
    double heading_degrees = 0.0;   // 11. Направление (Course Over Ground / Истинный курс, в градусах)
    
    // Новые параметры прикладного уровня AIS
    bool is_ais = false;            // Флаг детекции AIS-трафика
    uint32_t ais_msg_type = 0;      // 12. Тип сообщения AIS (числовой код 1, 2, 3, 4, 5, 18, 19, 27 по ITU-R M.1371)
    std::string ais_raw_payload;    // 13. Исходное сообщение AIS (чистый 6-битный ASCII блок из 5-го поля NMEA)
    
    // Рейсовые данные (заполняются только для AIS тип 5)
    std::string ais_vessel_name;    
    std::string ais_call_sign;
    std::string ais_destination;
    
    bool has_position = false;     
};
