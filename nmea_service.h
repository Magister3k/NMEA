#pragma once

#include "nmea_processor.h"
#include <thread>
#include <atomic>
#include <condition_variable>
#include <memory>

// Опережающее объявление, чтобы не раздувать инклуды в заголовке
class nmea450_decoder;

class nmea_service : public nmea_processor {
public:
    nmea_service();
    ~nmea_service();

    // Запрет копирования семантики (RAII-поток должен быть уникальным)
    nmea_service(const nmea_service&) = delete;
    nmea_service& operator=(const nmea_service&) = delete;

    /**
     * @brief Запуск фонового низкоприоритетного потока очистки таймаутов
     * @param net_meta_decoder Указатель на декодер L5, чьи таймауты сборки предложений (g:) тоже нужно чистить
     */
    void StartTimeoutCleaner(std::shared_ptr<nmea450_decoder> net_meta_decoder = nullptr);
    
    /**
     * @brief Принудительный останов фонового потока (вызывается также автоматически в деструкторе)
     */
    void StopTimeoutCleaner();

private:
    // Фоновое рабочее тело потока
    void CleanerWorker();

    std::thread m_cleaner_thread;
    std::atomic<bool> m_shutdown_requested{false};
    
    std::mutex m_cv_mutex;
    std::condition_variable m_cv;

    // Слабая ссылка на декодер NMEA-450 для безопасной очистки его пула из фонового потока
    std::shared_ptr<nmea450_decoder> m_net_meta_decoder = nullptr;
};
