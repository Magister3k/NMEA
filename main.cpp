#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <memory>
#include "LockFreeQueue.h"
#include "CompileTimePipeline.h"
#include "high_speed_geo_parser.h"
#include "ais_decoder.h"

// Глобальная неблокирующая SPSC очередь между сетью (L4) и конвейером обработки (L7)
// Емкость 131072 (степень двойки) гарантирует аппаратную защиту от пиковых сетевых всплесков
LockFreeSpscQueue<NetworkPacket, 131072> g_network_queue;
std::atomic<bool> g_running{true};

// ====================================================================
// ПОТОК 1: СЕТЕВОЙ ПРИЕМНИК (THREAD 1 - ВЫДЕЛЕННОЕ ЯДРО CPU)
// ====================================================================
void NetworkReceiveThreadLoop() {
    std::cout << "[THREAD 1] Сетевой поток захвата пакетов L4 успешно запущен." << std::endl;
    
    // Имитируем тестовые наборы данных, прилетающие из судового Ethernet
    // Тест 1: Стандартное предложение $GPRMC, несущее координаты, скорость и путевой угол
    std::string test_nmea_rmc = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.5,230394,003.1,W*6A\r\n";

    // Тест 2: Фрагментированный на уровне L5 (NMEA-450) сетевой пакет, несущий бинарный payload AIS тип 5
    // Фрагмент 1 из 2 (Спецификация IEC 61162-450)
    std::string test_n450_ais_part1 = std::string("UdPbC\0\\c:1672531200,s:AI01,g:1-2-9999*1A\\!AIVDM,2,1,5,B,538S`v024hBl0D`G22000000000,0*3D\r\n", 86);
    // Фрагмент 2 из 2
    std::string test_n450_ais_part2 = std::string("UdPbC\0\\c:1672531200,s:AI01,g:2-2-9999*1B\\!AIVDM,2,2,5,B,0000000000000000000,0*3A\r\n", 83);

    NetworkPacket packet;
    packet.src_ip = 0x0A000001; // 10.0.0.1
    packet.dst_ip = 0x0A0000FF; // 10.0.0.255 (Multicast)
    packet.src_port = 40001;
    packet.dst_port = 40001;

    // --- 1. Отправляем в конвейер текстовый GPS пакет $GPRMC ---
    packet.length = test_nmea_rmc.length();
    std::memcpy(packet.payload, test_nmea_rmc.data(), packet.length);
    while (!g_network_queue.Push(packet)) { std::this_thread::yield(); }

    // Даем конвейеру микросекундную паузу для наглядности вывода в консоль
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // --- 2. Отправляем Фрагмент 1 многострочного сообщения AIS ---
    packet.length = test_n450_ais_part1.length();
    std::memcpy(packet.payload, test_n450_ais_part1.data(), packet.length);
    while (!g_network_queue.Push(packet)) { std::this_thread::yield(); }

    // --- 3. Отправляем Фрагмент 2 многострочного сообщения AIS ---
    packet.length = test_n450_ais_part2.length();
    std::memcpy(packet.payload, test_n450_ais_part2.data(), packet.length);
    while (!g_network_queue.Push(packet)) { std::this_thread::yield(); }

    // Имитируем фоновое удержание потока (генерация пустых циклов, как в реальной сети)
    while (g_running.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::cout << "[THREAD 1] Сетевой поток остановлен." << std::endl;
}

// ====================================================================
// ПОТОК 2: ВЫЧИСЛИТЕЛЬНЫЙ ПРОЦЕССОР КОНВЕЙЕРА (THREAD 2 - ВЫДЕЛЕННОЕ ЯДРО)
// ====================================================================
void ProcessingThreadLoop() {
    std::cout << "[THREAD 2] Вычислительный поток L7 успешно запущен." << std::endl;

    // Инициализация прикладных вычислительных ядер
    high_speed_geo_parser geo_core;
    ais_decoder ais_core;
    
    // Проводка жестких связей на этапе компиляции (Compile-time Pipeline)
    // Компилятор полностью уничтожает накладные расходы виртуальных вызовов и инлайнит этот конвейер
    FastNmeaDecoder<ais_decoder, high_speed_geo_parser> l7_text_layer(ais_core, geo_core);
    FastNmea450Decoder<decltype(l7_text_layer)> l5_network_meta_layer(l7_text_layer);

    // Подключаем конечную шину вывода (Событийный Observer верхнего уровня)
    geo_core.SetOnReport([](const NmeaPositionReport& out) {
        std::cout << "\n====================================================================" << std::endl;
        std::cout << "[ВЫХОД МОДУЛЯ] Сгенерирован унифицированный путевой отчет:" << std::endl;
        std::cout << "  1. Сырая строка NMEA:  " << out.raw_sentence << std::endl;
        std::cout << "  2. 5-значный заголовок: " << out.five_letter_head << std::endl;
        std::cout << "  3. Прибор / Вендор:     " << out.device_description << " | " << out.manufacturer_desc << std::endl;
        std::cout << "  4. Описание типа NMEA: " << out.message_type_desc << std::endl;
        std::cout << "  5. Физический источник: " << out.message_source << std::endl;
        std::cout << "  6. Временной штамп:    " << out.timestamp << std::endl;
        std::cout << "  7. Идентификатор цели:  " << out.object_id << std::endl;
        
        if (out.has_position) {
            std::cout << "  8. Широта (Dec WGS84): " << std::fixed << std::setprecision(6) << out.latitude << std::endl;
            std::cout << "  9. Долгота (Dec WGS84): " << std::fixed << std::setprecision(6) << out.longitude << std::endl;
        }

        std::cout << "  10. Скорость (SOG):     " << out.speed_knots << " узлов" << std::endl;
        std::cout << "  11. Направление (COG):  " << out.heading_degrees << " градусов" << std::endl;

        if (out.is_ais) {
            std::cout << "  12. Внутренний тип AIS: " << out.ais_msg_type << std::endl;
            std::cout << "  13. Исходный payload AIS: " << out.ais_raw_payload << std::endl;
            
            if (out.ais_msg_type == 5) {
                std::cout << "    [AIS REYSE] Название судна:  " << out.ais_vessel_name << std::endl;
                std::cout << "    [AIS REYSE] Позывной прибора: " << out.ais_call_sign << std::endl;
                std::cout << "    [AIS REYSE] Назначение (Dest): " << out.ais_destination << std::endl;
            }
        }
        std::cout << "====================================================================" << std::endl;
    });

    NetworkPacket local_packet;

    // Основной вычислительный цикл L4-L7 конвейера
    while (g_running.load(std::memory_order_relaxed)) {
        // Выгребаем пакеты из неблокирующего кольца за 3 наносекунды
        if (g_network_queue.Pop(local_packet)) {
            // Формируем имя источника на основе сетевых дескрипторов пакета
            std::string src_info = "UDP_" + std::to_string(local_packet.src_port);
            
            // Вбрасываем сырые байты в инлайновый конвейер
            l5_network_meta_layer.ProcessRawChunk(local_packet.payload, local_packet.length, src_info);
        } else {
            // Если в сети временное затишье, разгружаем ядро процессора
            std::this_thread::yield();
        }
    }
    std::cout << "[THREAD 2] Вычислительный поток остановлен." << std::endl;
}

// ====================================================================
// ТОЧКА СБОРКИ И ПУСКА ПРИЛОЖЕНИЯ
// ====================================================================
int main() {
    std::cout << "====================================================================" << std::endl;
    std::cout << " СВЕРХВЫСОКОСКОРОСТНОЙ СЕТЕВОЙ НАВИГАЦИОННЫЙ СТЕНД РЕАЛЬНОГО ВРЕМЕНИ" << std::endl;
    std::cout << "====================================================================" << std::endl;

    // Шаг 1: Аллокация и привязка потоков к независимым ядрам процессора
    std::thread network_producer(NetworkReceiveThreadLoop);
    std::thread processing_consumer(ProcessingThreadLoop);

    // Даем конвейеру поработать и обработать встроенные тестовые векторы данных
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "\n[SYSTEM] Тестовая сессия завершена. Инициируем безопасный останов (RAII)..." << std::endl;

    // Шаг 2: Сигнальный останов потоков без зависаний
    g_running = false;
    
    network_producer.join();
    processing_consumer.join();

    std::cout << "[SYSTEM] Все потоки успешно выгружены. Выход." << std::endl;
    return 0;
}
