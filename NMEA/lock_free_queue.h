#pragma once
#include <atomic>
#include <vector>
#include <cstdint>
#include <new>

// ������������� ��������� ������ ��� ���������� ������������ ��������� � ����
struct NetPacket {
    uint32_t src_ip = 0;
    uint32_t dst_ip = 0;
    uint16_t src_port = 0;
    uint16_t dst_port = 0;
    uint8_t payload[512] = {0}; // ����� �������������� ������� (������� ��� ����� MTU NMEA)
    size_t len = 0;
};

template <typename T, size_t Capacity = 65536>
class LockFreeSpscQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity ������� ���� �������� ������!");
public:
    LockFreeSpscQueue() : m_write_idx(0), m_read_idx(0) {
        m_buffer.resize(Capacity);
    }

    // ��������� ������� (�������� ������� �����) - ������� �� �����������
    bool Push(const T& item) {
        const size_t cur_write = m_write_idx.load(std::memory_order_relaxed);
        const size_t cur_read = m_read_idx.load(std::memory_order_acquire);

        if ((cur_write - cur_read) == Capacity) {
            return false; // ������� ����������� (Drop ������)
        }

        m_buffer[cur_write & (Capacity - 1)] = item;
        m_write_idx.store(cur_write + 1, std::memory_order_release);
        return true;
    }

    // ��������� ������� (�������� �������������� �����) - ������� �� �����������
    bool Pop(T& item) {
        const size_t cur_read = m_read_idx.load(std::memory_order_relaxed);
        const size_t cur_write = m_write_idx.load(std::memory_order_acquire);

        if (cur_read == cur_write) {
            return false; // ������� �����
        }

        item = m_buffer[cur_read & (Capacity - 1)];
        m_read_idx.store(cur_read + 1, std::memory_order_release);
        return true;
    }

private:
    std::vector<T> m_buffer;
    
    // alignas(64) �������� ���������� � ������ ���-����� ���������� (��������� False Sharing)
    alignas(64) std::atomic<size_t> m_write_idx;
    alignas(64) std::atomic<size_t> m_read_idx;
};
