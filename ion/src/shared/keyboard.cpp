#include "keyboard.h"

#include <ion/keyboard.h>

#include "events.h"
#include "keyboard_queue.h"

namespace Ion {
namespace Keyboard {

State popState() {
  if (Queue::sharedQueue()->isEmpty()) {
    return State(-1);
  }
  return Queue::sharedQueue()->queuePop();
}

State sState(0);

void resetMemoizedState() { sState = 0; }

void pushState(State state) {
  Queue::sharedQueue()->push(state);
  sState = state;
}

void keyboardWasScanned(State state) {
  // The states are pushed on a queue and popped one at a time.
  if (state != sState) {
    Events::setPreemptiveKeyboardState(state);
    pushState(state);
  }
}

}  // namespace Keyboard
}  // namespace Ion
