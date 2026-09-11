#include "ais_decoder.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "[TEST] Запуск юнит-тестирования изолированного модуля ais_decoder..." << std::endl;

    // Создаем экземпляр чистого декодера
    ais_decoder decoder;

    bool position_test_passed = false;
    bool static_test_passed = false;

    // 1. Подписываемся на события навигационных отчетов судов
    decoder.SetOnPositionReport([&](const AisPositionReport& report) {
        std::cout << "-> Получен отчет о позиции (Тип " << report.message_type << ")" << std::endl;
        std::cout << "   MMSI: " << report.mmsi << std::endl;
        std::cout << "   Широта: " << report.latitude << " | Долгота: " << report.longitude << std::endl;
        std::cout << "   Скорость (SOG): " << report.sog << " узлов" << std::endl;
        
        // Верификация данных конкретного тестового пакета
        if (report.mmsi == 244670000) {
            position_test_passed = true;
        }
    });

    // 2. Подписываемся на события статических данных рейса
    decoder.SetOnStaticDataReport([&](const AisStaticDataReport& report) {
        std::cout << "-> Получены статические данные судна (Тип " << report.message_type << ")" << std::endl;
        std::cout << "   Название: " << report.ship_name << " | Позывной: " << report.call_sign << std::endl;
        std::cout << "   IMO номер: " << report.imo_number << std::endl;
        std::cout << "   Назначение: " << report.destination << std::endl;

        if (report.mmsi == 244670321) {
            static_test_passed = true;
        }
    });

    // --- ТЕСТ 1: Полезная нагрузка сообщения Тип 1 (Class A Position Report) ---
    // Исходная строка: !AIVDM,1,1,,A,133sVf0P00PDHRGOn7@Cw?vNP000,0*7F
    std::string test_payload_pos = "133sVf0P00PDHRGOn7@Cw?vNP000";
    std::cout << "\n[Запуск Теста 1: Декодирование координат]" << std::endl;
    decoder.DecodePayload(test_payload_pos);

    // --- ТЕСТ 2: Полезная нагрузка сообщения Тип 5 (Static Data) ---
    // В реальном проекте эта строка получается склеенной из двух фрагментов в модуле nmea450_decoder
    std::string test_payload_static = "538S`v024hBl0D`G220000000000000000000016000000000000000"; 
    std::cout << "\n[Запуск Теста 2: Декодирование названия и рейса]" << std::endl;
    decoder.DecodePayload(test_payload_static);

    // Итоговые проверки результатов
    std::cout << "\n==============================================" << std::endl;
    if (position_test_passed) {
        std::cout << "[Итог] Тест 1 (Позиция): УСПЕШНО ПРОЙДЕН." << std::endl;
    } else {
        std::cerr << "[Итог] Тест 1 (Позиция): СБОЙ!" << std::endl;
    }
    
    std::cout << "==============================================" << std::endl;
    return (position_test_passed) ? 0 : 1;
}
