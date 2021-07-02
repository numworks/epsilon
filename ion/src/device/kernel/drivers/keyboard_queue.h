#ifndef ION_DEVICE_KERNEL_KEYBOARD_QUEUE_H
#define ION_DEVICE_KERNEL_KEYBOARD_QUEUE_H

#include <ion/keyboard.h>

namespace Ion {
namespace Device {
namespace Keyboard {

class Queue {
public:
  Queue() : m_begin(0), m_end(0) {}
  static Queue * sharedQueue();
  void push(Ion::Keyboard::State s);
  Ion::Keyboard::State pop(); // crashes if empty
  bool isEmpty() const { return m_begin == m_end; }
  void flush(bool resetPending = true);
private:
  static constexpr size_t k_maximalNumberOfStates = 5;
  size_t clippedIncrement(size_t index) const;
  // Ring buffer
  Ion::Keyboard::State m_states[k_maximalNumberOfStates];
  size_t m_begin;
  size_t m_end;
};

}
}
}

#endif
