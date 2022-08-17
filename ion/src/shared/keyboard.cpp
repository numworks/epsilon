#include "keyboard.h"
#include "events.h"
#include "keyboard_queue.h"
#include <ion/keyboard.h>

namespace Ion {
namespace Keyboard {

State popState() {
  if (Queue::sharedQueue()->isEmpty()) {
    return State(-1);
  }
  return Queue::sharedQueue()->queuePop();
}

State sState(0);

void resetMemoizedState() {
  sState = 0;
}

void keyboardWasScanned(State state) {
  /* OnOff, Home and Back are the only keyboard keys which are preemptive.
   * The states are pushed on a queue and popped one at a time.
   * If the device is stalling, we do not queue the event to avoid a delayed
   * reaction. */
  Ion::Keyboard::Queue * queue = Ion::Keyboard::Queue::sharedQueue();
  if (state != sState && !queue->isFull()) {
    Events::setPendingKeyboardStateIfPreemtive(state);
    queue->push(state);
    sState = state;
  }
}

}
}
