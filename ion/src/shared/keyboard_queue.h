#ifndef ION_SHARED_KEYBOARD_QUEUE_H
#define ION_SHARED_KEYBOARD_QUEUE_H

#include <ion/keyboard.h>
#include <ion/ring_buffer.h>

namespace Ion {
namespace Keyboard {

constexpr static size_t k_maximalNumberOfStates = 5;

class Queue {
public:
  static Queue * sharedQueue();
  void flush(bool resetPending = true);

  void push(State s);
  State queuePop();
  size_t length() const { return m_buffer.length(); }
  bool isEmpty() const { return m_buffer.isEmpty(); }
  bool isFull() const { return m_buffer.isFull(); }


private:
  Queue() : m_lock(false) {}

  void reset();
  void didFlush(bool resetPending);

  RingBuffer<State, k_maximalNumberOfStates> m_buffer;
  bool m_lock;
};

}
}

#endif
