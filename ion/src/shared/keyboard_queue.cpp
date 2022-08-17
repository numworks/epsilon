#include "keyboard_queue.h"
#include "keyboard.h"
#include "events.h"

namespace Ion {
namespace Keyboard {

Queue * Queue::sharedQueue() {
  static Queue sQueue;
  return &sQueue;
}

void Queue::flush(bool resetPending) {
  reset();
  Keyboard::resetMemoizedState();
  Events::resetKeyboardState();
  if (resetPending) {
    Events::resetPendingKeyboardState();
  }
}

void Queue::push(State s) {
  if (!m_lock) {
    m_lock = true;
    m_buffer.push(s);
    m_lock = false;
  }
}

State Queue::queuePop() {
  if (!m_lock) {
    m_lock = true;
    State res = m_buffer.queuePop();
    m_lock = false;
    return res;
  }
  return State(-1);
}

void Queue::reset() {
  if (!m_lock) {
    m_lock = true;
    m_buffer.reset();
    m_lock = false;
  }
}

}
}
