#include "nmea_decoder.h"
#include <iostream>

int main() {
    std::cout << "[TEST] Тестирование изолированного модуля nmea_decoder..." << std::endl;

    nmea_decoder decoder;

    // Подписка на базовый разбор заголовков
    decoder.SetOnHeaderParsed([](const NmeaHeaderInfo& header) {
        std::cout << "[COLLBACK HEADER] Источник (Talker): " << header.talker_id 
                  << " | Спецификация: " << header.message_type << std::endl;
    });

    // Подписка на текстовые навигационные сообщения ($GPGGA)
    decoder.SetOnStandardMessage([](const std::string& talker, const std::string& type, const std::vector<std::string>& fields) {
        std::cout << "[CALLBACK TEXT NMEA] Прибор: " << talker << " | Тип: " << type << std::endl;
        if (type == "GGA" && fields.size() >= 4) {
            std::cout << "   -> Точное время UTC: " << fields[0] << std::endl;
            std::cout << "   -> Широта (сырая): " << fields[1] << " " << fields[2] << std::endl;
        }
    });

    // Подписка на АИС строки (перенаправление в ais_decoder)
    decoder.SetOnAisStringDetected([](const std::string& ais_payload) {
        std::cout << "[CALLBACK AIS STREAM] Выделен полезный payload: " << ais_payload << std::endl;
    });

    // Имитируем чтение строк из файла-лога судового регистра
    std::cout << "\n--- Считываем строку 1 (Запись спутников GPS) ---" << std::endl;
    decoder.ParseSentence("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47");

    std::cout << "\n--- Считываем строку 2 (Сырой пакет АИС) ---" << std::endl;
    decoder.ParseSentence("!AIVDM,1,1,,A,133sVf0P00PDHRGOn7@Cw?vNP000,0*7F");

    std::cout << "\n--- Считываем битую строку (Ошибка чексумы) ---" << std::endl;
    decoder.ParseSentence("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*99"); 
    std::cout << "Фильтрация завершена." << std::endl;

    return 0;
}
