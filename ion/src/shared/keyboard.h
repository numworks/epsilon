#ifndef ION_SHARED_KEYBOARD_H
#define ION_SHARED_KEYBOARD_H

#include <ion/keyboard.h>

namespace Ion {
namespace Keyboard {

State lastState();
void resetMemoizedState();
void pushState(State state);
void keyboardWasScanned(State state);

}  // namespace Keyboard
}  // namespace Ion

#endif
