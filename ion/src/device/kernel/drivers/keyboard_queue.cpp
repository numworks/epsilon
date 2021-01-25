#include <drivers/keyboard_queue.h>

namespace Ion {
namespace Device {
namespace Keyboard {

Queue * Queue::sharedQueue() {
  static Queue sQueue;
  return &sQueue;
}

size_t Queue::clippedIncrement(size_t index) const {
  return (index + 1) % k_maximalNumberOfStates;
}

void Queue::push(Ion::Keyboard::State s) {
  m_states[m_end] = s;
  m_end = clippedIncrement(m_end);
  m_begin = m_end == m_begin ? clippedIncrement(m_begin) : m_begin;
}

Ion::Keyboard::State Queue::pop() {
  assert(m_begin != m_end); // otherwise, queue is empty
  Ion::Keyboard::State s = m_states[m_begin];
  m_begin = clippedIncrement(m_begin);
  return s;
}

void Queue::flush() {
  m_begin = 0;
  m_end = 0;
}

}
}
}
