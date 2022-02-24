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

}
}
