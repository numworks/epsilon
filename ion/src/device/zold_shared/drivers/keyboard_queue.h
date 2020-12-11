#ifndef ION_DEVICE_SHARED_KEYBOARD_QUEUE_H
#define ION_DEVICE_SHARED_KEYBOARD_QUEUE_H

#include <ion/keyboard.h>
#include <stddef.h>

namespace Ion {
namespace Device {
namespace Keyboard {

using namespace Ion::Keyboard;

class Queue {
public:
  Queue() : m_begin(0), m_end(0) {}
  static Queue * sharedQueue();
  void push(State s);
  State pop(); // crashes if empty
  bool isEmpty() const { return m_begin == m_end; }
private:
  static constexpr size_t k_maximalNumberOfStates = 5;
  // Ring buffer
  State m_states[k_maximalNumberOfStates];
  size_t m_begin;
  size_t m_end;
};

}
}
}

#endif
