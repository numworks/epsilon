#include "keyboard_queue.h"

namespace Ion {
namespace Device {
namespace Keyboard {

Queue * Queue::sharedQueue() {
  static Queue sQueue;
  return &sQueue;
}

void Queue::push(Keyboard::State s) {
  m_states[m_end] = s;
  m_end = (m_end + 1) % k_maximalNumberOfStates;
  m_begin = m_end == m_begin ? m_begin + 1 : m_begin;
}

Keyboard::State Queue::pop() {
  assert(m_begin != m_end); // otherwise, queue is empty
  Keyboard::State s = m_states[m_begin];
  m_begin = (m_begin + 1) % k_maximalNumberOfStates;
  return s;
}

}
}
}
