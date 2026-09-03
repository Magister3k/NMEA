#include "nmea_coordinate_parser.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

// Обеспечиваем совместимость кроссплатформенного M_PI, если его нет в <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

nmea_coordinate_parser::nmea_coordinate_parser() {}

void nmea_coordinate_parser::SetOnCoordinateReport(CoordinateReportCallback cb) {
    m_report_cb = cb;
}

std::unordered_map<std::string, std::string> nmea_coordinate_parser::LoadFileToMap(const std::string& filepath) {
    std::unordered_map<std::string, std::string> dict;
    std::ifstream file(filepath);
    if (!file.is_open()) return dict;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; // Пропуск пустых строк и комментариев
        
        size_t sep_pos = line.find('=');
        if (sep_pos != std::string::npos) {
            std::string key = line.substr(0, sep_pos);
            std::string value = line.substr(sep_pos + 1);
            
            // Базовый тримминг символов возврата каретки \r для логов Windows
            if (!value.empty() && value.back() == '\r') value.pop_back();
            if (!key.empty() && key.back() == '\r') key.pop_back();
            
            dict[key] = value;
        }
    }
    return dict;
}

bool nmea_coordinate_parser::LoadDictionaries(const std::string& talker_file, 
                                              const std::string& prop_codes_file, 
                                              const std::string& msg_types_file) 
{
    m_talkers = LoadFileToMap(talker_file);
    m_manufacturers = LoadFileToMap(prop_codes_file);
    m_message_types = LoadFileToMap(msg_types_file);
    return (!m_talkers.empty() && !m_message_types.empty());
}

void nmea_coordinate_parser::Parse(const std::string& clean_sentence) {
    if (clean_sentence.length() < 6 || !m_report_cb) return;

    NmeaPositionReport report;
    report.raw_sentence = clean_sentence;

    // Изолируем тело от контрольной суммы для корректной токенизации по запятым
    size_t star_pos = clean_sentence.find('*');
    std::string body = clean_sentence.substr(0, star_pos);
    std::vector<std::string> fields = SplitString(body, ',');
    if (fields.empty()) return;

    std::string raw_header = fields[0]; // Например, "$GPGGA", "$PGRME", "$PRECT"
    if (raw_header.length() < 4) return;
    
    report.five_letter_head = raw_header.substr(0, 6);

    // Сценарий А: Обработка проприетарного формата (начинается на $P или !P)
    if (raw_header[1] == 'P' || raw_header[1] == 'p') {
        report.talker_description = "Proprietary (Vendor-Specific)";
        ParseProprietary(raw_header, fields, report);
    } 
    // Сценарий Б: Обработка стандартного международного формата NMEA 0183
    else {
        std::string talker_id = raw_header.substr(1, 2);
        std::string msg_type = raw_header.substr(3, 3);

        auto it_talker = m_talkers.find(talker_id);
        report.talker_description = (it_talker != m_talkers.end()) ? it_talker->second : "Unknown Talker Device";

        auto it_type = m_message_types.find(msg_type);
        report.message_type_desc = (it_type != m_message_types.end()) ? it_type->second : "Standard NMEA Sentence (" + msg_type + ")";
        report.manufacturer_desc = "N/A (Standard Specification)";

        if (msg_type == "GGA") ParseGGA(fields, report);
        else if (msg_type == "GLL") ParseGLL(fields, report);
        else if (msg_type == "RMC") ParseRMC(fields, report);
    }

    // Эмиссия унифицированной структуры на верхний уровень конвейера
    m_report_cb(report);
}

void nmea_coordinate_parser::ParseGGA(const std::vector<std::string>& fields, NmeaPositionReport& report) {
    if (fields.size() < 6 || fields[2].empty() || fields[4].empty()) return;
    report.latitude = ConvertToDecimalDegrees(fields[2], fields[3]);
    report.longitude = ConvertToDecimalDegrees(fields[4], fields[5]);
    report.has_position = true;
    report.object_id = "Internal GNSS Navigation Receiver";
}

void nmea_coordinate_parser::ParseGLL(const std::vector<std::string>& fields, NmeaPositionReport& report) {
    if (fields.size() < 5 || fields[1].empty() || fields[3].empty()) return;
    report.latitude = ConvertToDecimalDegrees(fields[1], fields[2]);
    report.longitude = ConvertToDecimalDegrees(fields[3], fields[4]);
    report.has_position = true;
    report.object_id = "Internal GNSS Navigation Receiver";
}

void nmea_coordinate_parser::ParseRMC(const std::vector<std::string>& fields, NmeaPositionReport& report) {
    if (fields.size() < 7 || fields[3].empty() || fields[5].empty()) return;
    if (fields[2] != "A") return; // А - данные валидны, V - предупреждение приемника

    report.latitude = ConvertToDecimalDegrees(fields[3], fields[4]);
    report.longitude = ConvertToDecimalDegrees(fields[5], fields[6]);
    report.has_position = true;
    report.object_id = "Internal GNSS Navigation Receiver";
}

void nmea_coordinate_parser::ParseProprietary(const std::string& header, const std::vector<std::string>& fields, NmeaPositionReport& report) {
    if (header.length() < 5) return;
    
    // Код изготовителя — первые 3 символа после $P (например, "GRM" - Garmin, "REC" - кастомный робот)
    std::string manufacturer_id = header.substr(2, 3);
    auto it_man = m_manufacturers.find(manufacturer_id);
    report.manufacturer_desc = (it_man != m_manufacturers.end()) ? it_man->second : "Custom / Unknown Manufacturer";

    std::string prop_type = (header.length() > 5) ? header.substr(5) : "";
    auto it_type = m_message_types.find(prop_type);
    report.message_type_desc = (it_type != m_message_types.end()) ? it_type->second : "Proprietary Data (" + prop_type + ")";

    // 🎯 ДЕТЕКЦИЯ ПРЯМОУГОЛЬНОЙ ТЕЛЕМЕТРИИ (МЕТРЫ)
    // Ожидаемый протокол: $PRECT,Easting(X),Northing(Y),Zone,Hemisphere*CC
    if (prop_type == "RECT" && fields.size() >= 5) {
        try {
            double easting = std::stod(fields[1]);
            double northing = std::stod(fields[2]);
            int zone = std::stoi(fields[3]);
            char hemisphere = (!fields[4].empty()) ? fields[4][0] : 'N';

            report.utm_zone = zone;
            report.utm_band = hemisphere;

            // Вызываем обратный математический движок: Плоские Метры -> Географические градусы WGS-84
            ConvertUtmToWgs84(easting, northing, zone, hemisphere, report);
            report.object_id = "Converted Telemetry Target";
        } catch (...) {
            report.has_position = false;
        }
    }
    // Стандартный фоллбек, если проприетарная строка несет гео-координаты по типу GGA
    else if (prop_type == "GGA" && fields.size() >= 6) {
        ParseGGA(fields, report);
    }
}

double nmea_coordinate_parser::ConvertToDecimalDegrees(const std::string& nmea_geo, const std::string& hemisphere) {
    if (nmea_geo.empty() || hemisphere.empty()) return 0.0;

    // Очистка от пробельных символов
    std::string geo = nmea_geo;
    geo.erase(0, geo.find_first_not_of(" \t"));
    geo.erase(geo.find_last_not_of(" \t") + 1);

    size_t dot_pos = geo.find('.');
    if (dot_pos == std::string::npos) return 0.0;

    bool is_longitude = (hemisphere == "E" || hemisphere == "W");
    size_t deg_digits = is_longitude ? 3 : 2;

    try {
        // Сценарий 1: Чистые десятичные градусы (DD.DDDDDD)
        if (dot_pos == deg_digits) {
            double decimal_degrees = std::stod(geo);
            if (hemisphere == "S" || hemisphere == "W") decimal_degrees = -decimal_degrees;
            return decimal_degrees;
        }

        // Сценарий 2: Формат секунд DDMMSS.SSS / DDDMMSS.SSS
        if (dot_pos == (deg_digits + 4)) {
            double degrees = std::stod(geo.substr(0, deg_digits));
            double minutes = std::stod(geo.substr(deg_digits, 2));
            double seconds = std::stod(geo.substr(deg_digits + 2));

            double decimal_degrees = degrees + (minutes / 60.0) + (seconds / 3600.0);
            if (hemisphere == "S" || hemisphere == "W") decimal_degrees = -decimal_degrees;
            return decimal_degrees;
        }

        // Сценарий 3: Классический NMEA 0183 (DDMM.MMMM)
        if (dot_pos == (deg_digits + 2)) {
            double degrees = std::stod(geo.substr(0, deg_digits));
            double minutes = std::stod(geo.substr(deg_digits));

            double decimal_degrees = degrees + (minutes / 60.0);
            if (hemisphere == "S" || hemisphere == "W") decimal_degrees = -decimal_degrees;
            return decimal_degrees;
        }

        // Фоллбек-защита: Динамический расчет компонентов от позиции разделительной точки
        if (dot_pos >= 2) {
            std::string min_part = geo.substr(dot_pos - 2); 
            std::string deg_part = geo.substr(0, dot_pos - 2);

            double degrees = deg_part.empty() ? 0.0 : std::stod(deg_part);
            double minutes = std::stod(min_part);

            double decimal_degrees = degrees + (minutes / 60.0);
            if (hemisphere == "S" || hemisphere == "W") decimal_degrees = -decimal_degrees;
            return decimal_degrees;
        }
    } catch (...) {
        return 0.0; // Защита от поврежденных символов в текстовом файле
    }
    return 0.0;
}

// ====================================================================
// 5. ОБРАТНЫЙ МАТЕМАТИЧЕСКИЙ ДВИЖОК: UTM (МЕТРЫ) -> WGS-84 (ГРАДУСЫ)
// ====================================================================

void nmea_coordinate_parser::ConvertUtmToWgs84(double easting, double northing, int zone, char hemisphere, NmeaPositionReport& report) {
    const double a = 6378137.0;           // Экваториальный радиус WGS-84 (в метрах)
    const double f = 1.0 / 298.257223563; // Сжатие земного сфероида
    const double k0 = 0.9996;             // Масштабный коэффициент на центральном меридиане

    const double e2 = 2.0 * f - f * f;
    const double e4 = e2 * e2;
    const double e6 = e4 * e2;
    const double ePrime2 = e2 / (1.0 - e2);

    // Учет инверсии знака для Южного полушария (False Northing)
    if (hemisphere == 'S' || hemisphere == 's') {
        northing -= 10000000.0;
    }

    double x = easting - 500000.0; // Срезаем False Easting сдвиг
    double y = northing;

    const double n = f / (2.0 - f);
    const double n2 = n * n;
    const double n3 = n * n2;
    const double n4 = n * n3;

    const double A = a * (1.0 + n2 / 4.0 + n4 / 64.0);
    const double M = y / k0;
    const double mu = M / A;

    const double beta1 = 3.0 * n / 2.0 - 27.0 * n3 / 32.0;
    const double beta2 = 21.0 * n2 / 16.0 - 55.0 * n4 / 32.0;
    const double beta3 = 151.0 * n3 / 96.0;
    const double beta4 = 1097.0 * n4 / 512.0;

    // Вычисляем сопряженную (Footprint) широту в радианах
    const double phi1_rad = mu + beta1 * std::sin(2.0 * mu) + beta2 * std::sin(4.0 * mu) + 
                                 beta3 * std::sin(6.0 * mu) + beta4 * std::sin(8.0 * mu);

    if (std::abs(phi1_rad) < 1e-10) {
        report.latitude = 0.0;
        report.longitude = ((zone - 1) * 6.0 - 180.0 + 3.0);
        report.has_position = true;
        return;
    }

    const double sin_phi1 = std::sin(phi1_rad);
    const double cos_phi1 = std::cos(phi1_rad);
    const double tan_phi1 = std::tan(phi1_rad);
    const double tan_phi12 = tan_phi1 * tan_phi1;
    const double tan_phi14 = tan_phi12 * tan_phi12;

    const double N1 = a / std::sqrt(1.0 - e2 * sin_phi1 * sin_phi1);
    const double R1 = a * (1.0 - e2) / std::pow(1.0 - e2 * sin_phi1 * sin_phi1, 1.5);
    const double D = x / (N1 * k0);
    const double D2 = D * D;

    const double C1 = ePrime2 * cos_phi1 * cos_phi1;
    const double T1 = tan_phi12;

    // Ряды Тейлора 6-й степени для прецизионного восстановления широты
    double lat_rad = phi1_rad - (N1 * tan_phi1 / R1) * (D2 / 2.0 - 
                     (5.0 + 3.0 * T1 + 10.0 * C1 - 4.0 * C1 * C1 - 9.0 * ePrime2) * D2 * D2 / 24.0 +
                     (61.0 + 90.0 * T1 + 298.0 * C1 + 45.0 * T1 * T1 - 252.0 * ePrime2 - 3.0 * C1 * C1) * D2 * D2 * D2 / 720.0);

    // Ряды Тейлора для прецизионного восстановления долготы
    double lon_diff_rad = (D - (1.0 + 2.0 * T1 + C1) * D2 * D / 6.0 +
                          (5.0 - 2.0 * C1 + 28.0 * T1 - 3.0 * C1 * C1 + 8.0 * ePrime2 + 24.0 * T1 * T1) * D2 * D2 * D / 120.0) / cos_phi1;

    // Восстанавливаем долготу относительно центрального меридиана расчетной зоны
    const double lon0_rad = ((zone - 1) * 6.0 - 180.0 + 3.0) * M_PI / 180.0;
    double lon_rad = lon0_rad + lon_diff_rad;

    report.latitude = lat_rad * 180.0 / M_PI;
    report.longitude = lon_rad * 180.0 / M_PI;
    
    // Циклическая нормализация долготы в диапазон [-180.0, 180.0] градусов
    if (report.longitude > 180.0) report.longitude -= 360.0;
    if (report.longitude < -180.0) report.longitude += 360.0;

    report.has_position = true;
}

std::vector<std::string> nmea_coordinate_parser::SplitString(const std::string& str, char delimiter) const {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    if (!str.empty() && str.back() == delimiter) {
        tokens.push_back("");
    }
    return tokens;
}
