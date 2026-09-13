#pragma once
#include <string>
#include <cstdint>

/**
 * @brief Промежуточная структура динамических данных позиционирования судна AIS
 *        Применяется для декодирования сообщений типов 1, 2, 3 (Class A Position Report),
 *        типа 4 (Base Station), типов 18/19 (Class B) и типа 27 (Satellite Long-Range).
 */
struct AisPosReport {
    int msg_type = 0;         // Тип сообщения (1-4, 18, 19, 27)
    uint32_t mmsi = 0;        // MMSI
    int nav_status = 15;      // Навигационный статус (0 - на ходу, 1 - на якоре, ..., 15 - не определен)
    double rot = 0.0;         // Скорость поворота (Rate of Turn)
    double sog = 0.0;         // Скорость (Speed Over Ground) в узлах
    bool pos_accurate = false;// Точность позиционирования (DGPS / обычный GNSS)
    double lon = 0.0;         // Долгота в десятичных градусах (WGS-84)
    double lat = 0.0;         // Широта в десятичных градусах (WGS-84)
    double cog = 0.0;         // Курс (Course Over Ground) в градусах
    int true_heading = 511;   // Истинный курс в градусах (0-359, 511 - данные недоступны)
    uint32_t timestamp = 60;  // Временная метка UTC в секундах (0-59, 60 - данные недоступны)
};

/**
 * @brief Промежуточная структура статических и рейсовых данных судна AIS
 *        Применяется для декодирования тяжелых сообщений типа 5 (Static and Voyage Related Data),
 *        которые обычно приходят фрагментированными на уровне L5 (NMEA-450).
 */
struct AisDataReport {
    int msg_type = 5;          // Всегда 5 для данной спецификации
    uint32_t mmsi = 0;         // MMSI
    uint32_t ais_ver = 0;      // Версия протокола AIS
    uint32_t imo_num = 0;      // Номер IMO
    std::string call_sign;     // Позывной судна
    std::string ship_name;     // Название судна
    int ship_type = 0;         // Тип судна и характер перевозимого груза (коды ITU)
    
    // Метрические габариты судна относительно установленной антенны ГНСС
    uint32_t dim_to_bow = 0;       // Расстояние от антенны до носа (в метрах)
    uint32_t dim_to_stern = 0;     // Расстояние от антенны до кормы (в метрах)
    uint32_t dim_to_port = 0;      // Расстояние от антенны до левого борта (в метрах)
    uint32_t dim_to_starboard = 0; // Расстояние от антенны до правого борта (в метрах)
    
    int pos_fix_type = 0; // Тип навигационного оборудования (GPS, GLONASS, Loran и др.)
    std::string dest;     // Порт назначения
    
    // Расчетное время прибытия (ETA)
    uint32_t eta_month = 0;        
    uint32_t eta_day = 0;          
    uint32_t eta_hour = 0;         
    uint32_t eta_minute = 0;       
};
