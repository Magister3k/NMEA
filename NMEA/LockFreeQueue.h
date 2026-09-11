#pragma once
#include <atomic>
#include <vector>
#include <cstdint>
#include <new>

// Фиксированная структура пакета для исключения динамической аллокации в куче
struct NetworkPacket {
    uint32_t src_ip = 0;
    uint32_t dst_ip = 0;
    uint16_t src_port = 0;
    uint16_t dst_port = 0;
    uint8_t payload[512] = {0}; // Буфер фиксированного размера (хватает под любой MTU NMEA)
    size_t length = 0;
};

template <typename T, size_t Capacity = 65536>
class LockFreeSpscQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity обязана быть степенью двойки!");
public:
    LockFreeSpscQueue() : m_write_idx(0), m_read_idx(0) {
        m_buffer.resize(Capacity);
    }

    // Вставляет элемент (вызывает Сетевой Поток) - Никогда не блокируется
    bool Push(const T& item) {
        const size_t current_write = m_write_idx.load(std::memory_order_relaxed);
        const size_t current_read = m_read_idx.load(std::memory_order_acquire);

        if ((current_write - current_read) == Capacity) {
            return false; // Очередь переполнена (Drop пакета)
        }

        m_buffer[current_write & (Capacity - 1)] = item;
        m_write_idx.store(current_write + 1, std::memory_order_release);
        return true;
    }

    // Извлекает элемент (вызывает Вычислительный Поток) - Никогда не блокируется
    bool Pop(T& item) {
        const size_t current_read = m_read_idx.load(std::memory_order_relaxed);
        const size_t current_write = m_write_idx.load(std::memory_order_acquire);

        if (current_read == current_write) {
            return false; // Очередь пуста
        }

        item = m_buffer[current_read & (Capacity - 1)];
        m_read_idx.store(current_read + 1, std::memory_order_release);
        return true;
    }

private:
    std::vector<T> m_buffer;
    
    // alignas(64) разносит переменные в разные кэш-линии процессора (устраняет False Sharing)
    alignas(64) std::atomic<size_t> m_write_idx;
    alignas(64) std::atomic<size_t> m_read_idx;
};
