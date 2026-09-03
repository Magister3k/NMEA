#include "nmea450_decoder.h"
#include <iostream>
#include <vector>

int main() {
    std::cout << "[TEST] Тестирование изолированного модуля nmea450_decoder..." << std::endl;

    nmea450_decoder decoder;

    // Подписываемся на выходной поток очищенных строк
    decoder.SetOnSentenceAssembled([](const std::string& clean_sentence, const std::string& source_id) {
        std::cout << "\n[CALLBACK SUCCESS] Сообщение успешно собрано!" << std::endl;
        std::cout << "   -> Физический прибор: " << source_id << std::endl;
        std::cout << "   -> Итоговая NMEA строка: " << clean_sentence << std::endl;
    });

    // 1. Симулируем прохождение одиночного пакета (например, эхолота)
    std::string single_packet = std::string("UdPbC\0\\c:1672531199,s:SD001*41\\$SDDPT,14.2,0.0*57", 49);
    std::cout << "\n--- Отправляем одиночную датаграмму Эхолота ---" << std::endl;
    decoder.ProcessPacket(reinterpret_cast<const uint8_t*>(single_packet.data()), single_packet.length());

    // 2. Симулируем прохождение многострочного пакета АИС (2 фрагмента, тип 5)
    // Строка 1
    std::string packet_part1 = std::string("UdPbC\0\\c:1672531200,s:AI01,g:1-2-8888*1A\\!AIVDM,2,1,5,B,538S`v024hBl0D`G22000000000,0*3D", 84);
    // Строка 2
    std::string packet_part2 = std::string("UdPbC\0\\c:1672531200,s:AI01,g:2-2-8888*1B\\!AIVDM,2,2,5,B,0000000000000000000,0*3A", 81);

    std::cout << "\n--- Отправляем Фрагмент 1 многострочного сообщения АИС ---" << std::endl;
    decoder.ProcessPacket(reinterpret_cast<const uint8_t*>(packet_part1.data()), packet_part1.length());
    std::cout << "[INFO] Буфер зафиксирован, ожидаем завершения сборки..." << std::endl;

    std::cout << "\n--- Отправляем Фрагмент 2 многострочного сообщения АИС ---" << std::endl;
    decoder.ProcessPacket(reinterpret_cast<const uint8_t*>(packet_part2.data()), packet_part2.length());

    return 0;
}
