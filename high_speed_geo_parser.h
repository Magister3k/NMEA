#pragma once

#include <string>
#include <vector>
#include <functional>
#include "NmeaPositionStructures.h"

class high_speed_geo_parser {
public:
    // Колбэк верхнего уровня для выдачи результата в бизнес-логику (Observer)
    using Callback = std::function<void(const NmeaPositionReport&)>;

    high_speed_geo_parser() = default;
    ~high_speed_geo_parser() = default;

    // Запрет копирования и перемещения (RAII / Память конвейера должна быть монолитной)
    high_speed_geo_parser(const high_speed_geo_parser&) = delete;
    high_speed_geo_parser& operator=(const high_speed_geo_parser&) = delete;

    /**
     * @brief Регистрация конечного обработчика унифицированных гео-отчетов
     */
    void SetOnReport(Callback cb);

    /**
     * @brief Главная инлайновая точка входа текстового конвейера (L7)
     * @param header Ссылка на заголовочное поле (например, "$GPGGA")
     * @param fields Вектор токенизированных полей сообщения
     * @param raw Полная исходная сырая строка NMEA
     */
    void ParseIncomingSentence(const std::string& header, const std::vector<std::string>& fields, const std::string& raw);

private:
    // Разбор стандартных спецификаций NMEA 0183
    void ParseStandard(const std::string& type, const std::vector<std::string>& f, NmeaPositionReport& r);

    // Разбор вендорских и военных проприетарных протоколов ($P...)
    void ParseProprietary(const std::string& h, const std::string& t, const std::vector<std::string>& f, NmeaPositionReport& r);

    /**
     * @brief Прецизионный разбор и конвертация угловых минут NMEA без heap-аллокаций.
     *        Оптимизирован под инлайнинг и векторизацию -ffast-math.
     */
    double GeoToDec(const std::string& nmea, const std::string& hemi);

    /**
     * @brief Обратный прецизионный геодезический движок: Метры UTM -> Градусы WGS-84
     *        Реализован на рядах Тейлора 6-й степени с поддержкой аппаратного FMA.
     */
    void ConvertUtmToWgs84(double easting, double northing, int zone, char hemisphere, NmeaPositionReport& report);

    Callback m_cb = nullptr;
};
