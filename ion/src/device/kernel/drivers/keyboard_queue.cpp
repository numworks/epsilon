#include <drivers/keyboard.h>
#include <drivers/keyboard_queue.h>
#include <drivers/events.h>

namespace Ion {
namespace Device {
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
}
