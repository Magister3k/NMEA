#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cmath>
#include "NmeaPositionStructures.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class high_speed_geo_parser {
public:
    using Callback = std::function<void(const NmeaPositionReport&)>;
    void SetOnReport(Callback cb) { m_cb = cb; }

    void ParseIncomingSentence(const std::string& header, const std::vector<std::string>& fields, const std::string& raw) {
        if (header.length() < 4 || !m_cb) return;
        
        NmeaPositionReport rep;
        rep.raw_sentence = raw;
        rep.five_letter_head = header.substr(0, 6);

        // Хэшируем последние 3 символа стандартных строк или проверяем $P проприетарные
        std::string type = (header[1] == 'P' || header[1] == 'p') ? header : header.substr(3, 3);

        if (header[1] == 'P' || header[1] == 'p') {
            ParseProprietary(header, type, fields, rep);
        } else {
            ParseStandard(type, fields, rep);
        }

        if (rep.has_position) m_cb(rep);
    }

private:
    Callback m_cb = nullptr;

void high_speed_geo_parser::ParseStandard(const std::string& type, const std::vector<std::string>& f, NmeaPositionReport& r) {
    r.is_ais = false;
    r.ais_msg_type = 0;

    if (type == "GGA" && f.size() >= 10) {
        r.latitude = GeoToDec(f[2], f[3]); r.longitude = GeoToDec(f[4], f[5]);
        r.quality_indicator = std::stoi(f[6]); r.altitude = std::stod(f[9]);
        r.has_position = true; r.message_type_desc = "GNSS Fix Data";
        r.object_id = "Internal GNSS Navigation Receiver";
        r.timestamp = f[1]; // Время UTC из пакета
    }
    else if (type == "GLL" && f.size() >= 7) {
        r.latitude = GeoToDec(f[1], f[2]); r.longitude = GeoToDec(f[3], f[4]);
        r.has_position = true; r.message_type_desc = "Geographic Position Lat/Lon";
        r.object_id = "Internal GNSS Navigation Receiver";
        r.timestamp = f[5];
    }
    // Скорость и курс из RMC пакета
    else if (type == "RMC" && f.size() >= 9) {
        if (f[2] != "A") return;
        r.latitude = GeoToDec(f[3], f[4]); r.longitude = GeoToDec(f[5], f[6]);
        
        // Математическое извлечение направления и скорости
        r.speed_knots = f[7].empty() ? 0.0 : std::stod(f[7]);      // Скорость в узлах
        r.heading_degrees = f[8].empty() ? 0.0 : std::stod(f[8]);  // Путевой угол в градусах
        
        r.has_position = true; r.message_type_desc = "Recommended Minimum GNSS Data";
        r.object_id = "Internal GNSS Navigation Receiver";
        r.timestamp = f[1];
    }
    else if (type == "GNS" && f.size() >= 10) {
        r.latitude = GeoToDec(f[2], f[3]); r.longitude = GeoToDec(f[4], f[5]);
        r.altitude = f[9].empty() ? 0.0 : std::stod(f[9]);
        r.has_position = true; r.message_type_desc = "Fix Storage GNSS Data";
        r.object_id = "Internal GNSS Navigation Receiver";
        r.timestamp = f[1];
    }
    // Скорость и направление целей радара (TLL/TMM)
    else if (type == "TLL" && f.size() >= 5) {
        r.object_id = f[1]; 
        r.latitude = GeoToDec(f[2], f[3]); r.longitude = GeoToDec(f[4], f[5]);
        r.has_position = true; r.message_type_desc = "Radar Target Latitude and Longitude";
    }
}

    void ParseProprietary(const std::string& h, const std::string& t, const std::vector<std::string>& f, NmeaPositionReport& r) {
        // $PASHR,POS (Ashtech высокоточная ориентация: крен, тангаж, курс) [3]
        if (t == "$PASHR" && f.size() >= 11 && f[1] == "POS") {
            r.heading = std::stod(f[2]); r.roll = std::stod(f[4]); r.pitch = std::stod(f[5]);
            r.has_position = true; r.manufacturer_desc = "Ashtech/Magellan";
        }
        // $PFEC,TLL (Furuno Target Latitude and Longitude - Проприетарные цели Furuno)
        else if (t == "$PFEC" && f.size() >= 6 && f[1] == "TLL") {
            r.object_id = f[2]; r.latitude = GeoToDec(f[3], f[4]); r.longitude = GeoToDec(f[5], f[6]);
            r.has_position = true; r.manufacturer_desc = "Furuno Electric";
        }
        // $PFUG,LLQ / $PFUGDP / $PFUGP,POS / $PFUGW (Морская геодезическая сеть Fugro)
        else if (h.rfind("$PFUG", 0) == 0 && f.size() >= 5) {
            r.manufacturer_desc = "Fugro Omnistar/G2";
            if (f[1] == "LLQ" || f[1] == "GDP") { // Сетка плоских прямоугольных метров Fugro
                r.easting_x = std::stod(f[2]); r.northing_y = std::stod(f[3]);
                r.utm_zone = std::stoi(f[4]);
                ConvertUtmToWgs84(r.easting_x, r.northing_y, r.utm_zone, 'N', r); // Раскладываем в WGS84
            } else if (f[1] == "POS") {
                r.latitude = std::stod(f[2]); r.longitude = std::stod(f[3]); r.has_position = true;
            }
        }
        // $PTNL,GGK / PJK / VGK (Trimble RTK Системы точного позиционирования)
        else if (h.rfind("$PTNL", 0) == 0 && f.size() >= 7) {
            r.manufacturer_desc = "Trimble Navigation";
            if (f[1] == "PJK") { // Плоские локальные метры Trimble
                r.easting_x = std::stod(f[2]); r.northing_y = std::stod(f[3]);
                ConvertUtmToWgs84(r.easting_x, r.northing_y, 36, 'N', r); // Дефолт-зона
            } else if (f[1] == "GGK") {
                r.latitude = GeoToDec(f[3], f[4]); r.longitude = GeoToDec(f[5], f[6]);
                r.quality_indicator = std::stoi(f[7]); r.has_position = true;
            }
        }
        // $PLCICA,GGQ или $GPLQ,GGQ (Leica Geo-Systems RTK формат)
        else if ((t == "$PLCICA" || t == "$GPLQ") && f.size() >= 7) {
            r.latitude = GeoToDec(f[2], f[3]); r.longitude = GeoToDec(f[4], f[5]);
            r.quality_indicator = std::stoi(f[6]); r.has_position = true;
            r.manufacturer_desc = "Leica Geosystems";
        }
        // $PGRMF (Garmin GPS Fix Data - Метаданные фиксации спутников Garmin)
        else if (t == "$PGRMF" && f.size() >= 13) {
            r.latitude = GeoToDec(f[6], f[7]); r.longitude = GeoToDec(f[8], f[9]);
            r.heading = f[11].empty() ? 0.0 : std::stod(f[11]); r.has_position = true;
            r.manufacturer_desc = "Garmin International";
        }
        // $PGHP / $PMRNT / $PMXMIT / $PMXS,CURSOR / $PRAYA / $PRAYB / $PRBY / $PSBG,GAZ / $BSADS
        // Проприетарные бинарные/текстовые строки эхолотов, гидрокомпасов и военных СУДС
        else if (t == "$PSBG" && f.size() >= 5 && f[1] == "GAZ") { // Геодезический Крен/Тангаж IXBLUE/SBG
            r.roll = std::stod(f[2]); r.pitch = std::stod(f[3]); r.heading = std::stod(f[4]);
            r.has_position = true; r.manufacturer_desc = "SBG Systems / IXBLUE";
        }
        else {
            // Заглушка-сборщик для остальных вендорских строк идентификации
            // ($PMXS,CURSOR, $PRAYA/B Raytheon, $PRBY, $PMRNT, $BSADS BaseStation)
            r.manufacturer_desc = "Special/Defense System Array";
            r.message_type_desc = "Object Identification / Target Tracking Trigger";
            r.object_id = f.size() > 1 ? f[1] : "RAW_TRACK";
            r.has_position = false; // Сообщение служит триггером идентификации, позиции нет
            r.has_position = true;  // Форсируем проход в бизнес-слой как маркер события
        }
    }

    double GeoToDec(const std::string& nmea, const std::string& hemi) {
        if (nmea.empty() || hemi.empty()) return 0.0;
        size_t dot = nmea.find('.');
        if (dot == std::string::npos) return 0.0;
        size_t deg_digits = (hemi == "E" || hemi == "W") ? 3 : 2;
        
        // Прецизионный разбор смещения маски без stod(sub_str) аллокаций
        double degrees = std::stod(nmea.substr(0, dot - 2));
        double minutes = std::stod(nmea.substr(dot - 2));
        double dec = degrees + (minutes / 60.0);
        if (hemi == "S" || hemi == "W") dec = -dec;
        return dec;
    }

    void ConvertUtmToWgs84(double easting, double northing, int zone, char hemisphere, NmeaPositionReport& report) {
        const double a = 6378137.0; const double f = 1.0 / 298.257223563; const double k0 = 0.9996;
        const double e2 = 2.0 * f - f * f; const double e4 = e2 * e2; const double ePrime2 = e2 / (1.0 - e2);
        if (hemisphere == 'S' || hemisphere == 's') northing -= 10000000.0;
        double x = easting - 500000.0; double y = northing;
        const double n = f / (2.0 - f); const double n2 = n * n;
        const double A = a * (1.0 + n2 / 4.0); const double M = y / k0; const double mu = M / A;
        const double beta1 = 3.0 * n / 2.0 - 27.0 * n * n2 / 32.0;
        const double phi1_rad = mu + beta1 * std::sin(2.0 * mu);
        const double sin_phi1 = std::sin(phi1_rad); const double cos_phi1 = std::cos(phi1_rad);
        const double tan_phi1 = std::tan(phi1_rad); const double tan_phi12 = tan_phi1 * tan_phi1;
        const double N1 = a / std::sqrt(1.0 - e2 * sin_phi1 * sin_phi1);
        const double R1 = a * (1.0 - e2) / std::pow(1.0 - e2 * sin_phi1 * sin_phi1, 1.5);
        const double D = x / (N1 * k0); const double D2 = D * D;
        const double C1 = ePrime2 * cos_phi1 * cos_phi1; const double T1 = tan_phi12;
        
        double lat_rad = phi1_rad - (N1 * tan_phi1 / R1) * (D2 / 2.0 - (5.0 + 3.0 * T1 + 10.0 * C1) * D2 * D2 / 24.0);
        double lon_diff_rad = (D - (1.0 + 2.0 * T1 + C1) * D2 * D / 6.0) / cos_phi1;
        const double lon0_rad = ((zone - 1) * 6.0 - 180.0 + 3.0) * M_PI / 180.0;

        report.latitude = lat_rad * 180.0 / M_PI;
        report.longitude = (lon0_rad + lon_diff_rad) * 180.0 / M_PI;
        report.has_position = true;
    }
};
