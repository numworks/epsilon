#ifndef ION_SHARED_KEYBOARD_H
#define ION_SHARED_KEYBOARD_H

#include <ion/keyboard.h>

namespace Ion {
namespace Keyboard {

void resetMemoizedState();
void keyboardWasScanned(State state);

}
}

#endif
