#include "nmea_service.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <sys/resource.h>
#endif

nmea_service::nmea_service() {}

nmea_service::~nmea_service() {
    StopTimeoutCleaner();
}

void nmea_service::StartTimeoutCleaner() {
    if (m_cleaner_thread.joinable()) return; // Управление повторным запуском

    m_shutdown_requested = false;
    m_cleaner_thread = std::thread(&nmea_service::CleanerWorker, this);
}

void nmea_service::StopTimeoutCleaner() {
    m_shutdown_requested = true;
    m_cv.notify_all(); // Мгновенно будим поток, если он спал

    if (m_cleaner_thread.joinable()) {
        m_cleaner_thread.join();
    }
}

void nmea_service::CleanerWorker() {
    // Понижаем приоритет фонового потока на уровне ядра ОС.
    // Это гарантирует, что сетевые потоки всегда вытеснят этот поток при нехватке CPU.
#ifdef _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#else
    // Для Linux (POSIX) выставляем значение nice (от -20 до 19, где 19 - самый низкий приоритет)
    setpriority(PRIO_PROCESS, 0, 10); 
#endif

    // Будем проводить очистку каждые 2 секунды
    const auto interval = std::chrono::seconds(2);

    while (!m_shutdown_requested) {
        std::unique_lock<std::mutex> lock(m_cv_mutex);
        
        // Вместо глухого std::this_thread::sleep_for мы используем condition_variable.
        // Это позволяет потоку мгновенно проснуться и завершиться при закрытии приложения,
        // а не зависать на 2 секунды в памяти.
        if (m_cv.wait_for(lock, interval, [this]() { return m_shutdown_requested.load(); })) {
            break; // Проснулись из-за shutdown_requested -> выходим
        }

        // Вызываем логику очистки из базового класса.
        // Мьютекс m_mutex внутри CleanupTimeouts заблокируется на очень короткий срок,
        // так как операции удаления элементов из unordered_map по итератору эффективны.
        CleanupTimeouts();
    }
}
