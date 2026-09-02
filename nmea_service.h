#pragma once

#include "nmea_processor.h"
#include <thread>
#include <atomic>
#include <condition_variable>

class nmea_service : public nmea_processor {
public:
    nmea_service();
    ~nmea_service();

    // Запуск фоновой очистки
    void StartTimeoutCleaner();
    
    // Принудительный останов (также вызывается в деструкторе)
    void StopTimeoutCleaner();

private:
    void CleanerWorker();

    std::thread m_cleaner_thread;
    std::atomic<bool> m_shutdown_requested{false};
    std::mutex m_cv_mutex;
    std::condition_variable m_cv;
};
