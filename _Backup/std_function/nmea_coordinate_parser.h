#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "NmeaPositionStructures.h"

class nmea_coordinate_parser {
public:
    using CoordinateReportCallback = std::function<void(const NmeaPositionReport&)>;

    nmea_coordinate_parser();
    ~nmea_coordinate_parser() = default;

    // Запрет копирования
    nmea_coordinate_parser(const nmea_coordinate_parser&) = delete;
    nmea_coordinate_parser& operator=(const nmea_coordinate_parser&) = delete;

    // Регистрация колбэка
    void SetOnCoordinateReport(CoordinateReportCallback cb);

    /**
     * @brief Загрузка справочников из текстовых файлов
     * @return true, если все критические файлы успешно прочитаны
     */
    bool LoadDictionaries(const std::string& talker_file, 
                          const std::string& prop_codes_file, 
                          const std::string& msg_types_file);

    /**
     * @brief Разбор очищенной NMEA строки на составляющие
     * @param clean_sentence Строка NMEA, прошедшая валидацию контрольной суммы
     */
    void Parse(const std::string& clean_sentence);

private:
    // Внутренние парсеры конкретных подтипов NMEA
    void ParseGGA(const std::vector<std::string>& fields, NmeaPositionReport& report);
    void ParseGLL(const std::vector<std::string>& fields, NmeaPositionReport& report);
    void ParseRMC(const std::vector<std::string>& fields, NmeaPositionReport& report);
    void ParseProprietary(const std::string& header, const std::vector<std::string>& fields, NmeaPositionReport& report);

    // Утилиты конвертации географических координат NMEA (DDMM.MMMM -> Десятичные градусы)
    double ConvertToDecimalDegrees(const std::string& nmea_geo, const std::string& hemisphere);
    std::unordered_map<std::string, std::string> LoadFileToMap(const std::string& filepath);
    std::vector<std::string> SplitString(const std::string& str, char delimiter) const;

    /**
     * @brief Разбор кастомной проприетарной строки, содержащей метры (X, Y, Зона)
     */
    void ParseRectratangularSentence(const std::vector<std::string>& fields, NmeaPositionReport& report);

    /**
     * @brief Математический обратный пересчет: плоские метры UTM -> Географические WGS-84 (Широта, Долгота)
     * @param easting Координата X (Восток), в метрах
     * @param northing Координата Y (Север), в метрах
     * @param zone Номер зоны UTM (1-60)
     * @param hemisphere Полушарие ('N' или 'S')
     * @param[out] report Ссылка на отчет для записи вычисленной широты и долготы
     */
    void ConvertUtmToWgs84(double easting, double northing, int zone, char hemisphere, NmeaPositionReport& report);


    CoordinateReportCallback m_report_cb = nullptr;

    // Словари метаданных
    std::unordered_map<std::string, std::string> m_talkers;
    std::unordered_map<std::string, std::string> m_manufacturers;
    std::unordered_map<std::string, std::string> m_message_types;
};
