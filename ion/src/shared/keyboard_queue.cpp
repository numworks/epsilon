#include "keyboard_queue.h"

#include "events.h"
#include "keyboard.h"

namespace Ion {
namespace Keyboard {

Queue* Queue::sharedQueue() {
  static Queue sQueue;
  return &sQueue;
}

void Queue::flush() { reset(); }

void Queue::push(State s) {
  if (m_busy) {
    m_pushedWhileBusy = s;
  } else {
    m_busy = true;
    m_buffer.push(s);
    m_busy = false;
    handleBusyState();
  }
}

State Queue::queuePop() {
  assert(!m_busy);
  m_busy = true;
  State res = m_buffer.queuePop();
  m_busy = false;
  handleBusyState();
  return res;
}

void Queue::reset() {
  assert(!m_busy);
  m_busy = true;
  m_buffer.reset();
  m_busy = false;
  m_pushedWhileBusy = State(-1);
}

void Queue::handleBusyState() {
  if (m_pushedWhileBusy != State(-1)) {
    State s = m_pushedWhileBusy;
    m_pushedWhileBusy = State(-1);
    push(s);
  }
}

}  // namespace Keyboard
}  // namespace Ion
