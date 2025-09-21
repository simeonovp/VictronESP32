#include <Arduino.h>
#include <atomic>
#include "config.h"

class LockFreeStringQueue
{
public:
  static const uint8_t SIZE = MAX_BLOCK_COUNT * MAX_KEY_VALUE_COUNT;
  static_assert(256u >= SIZE, "Queue size too large");
  LockFreeStringQueue() : head(0), tail(0) {}

  // Returns false, if queue full
  bool enqueue(const String& s)
  {
    uint8_t next = (head + 1) % SIZE;
    if (next == tail.load(std::memory_order_acquire))
    {
      return false; // Queue voll
    }
    buffer[head] = s;
    head.store(next, std::memory_order_release);
    return true;
  }

  // Returns false, if queue empty
  bool dequeue(String& out)
  {
    if (tail.load(std::memory_order_acquire) == head.load(std::memory_order_acquire))
    {
      return false; // leer
    }
    out = buffer[tail];
    tail.store((tail + 1) % SIZE, std::memory_order_release);
    return true;
  }

  bool isEmpty() const
  {
    return head.load() == tail.load();
  }

  bool isFull() const
  {
    return ((head + 1) % SIZE) == tail.load();
  }

private:
  String buffer[SIZE];
  std::atomic<uint8_t> head;
  std::atomic<uint8_t> tail;
};