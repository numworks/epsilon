#pragma once

#include <ion/keyboard.h>

namespace Ion {
namespace Keyboard {

State lastState();
void resetMemoizedState();
void pushState(State state);
void keyboardWasScanned(State state);

}  // namespace Keyboard
}  // namespace Ion
