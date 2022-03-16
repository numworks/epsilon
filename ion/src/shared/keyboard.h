#ifndef ION_SHARED_KEYBOARD_H
#define ION_SHARED_KEYBOARD_H

#include <ion/keyboard.h>

namespace Ion {
namespace Keyboard {

void willPopState(); // Callback only implemented on simulator, to refresh the window
void resetMemoizedState();
void keyboardWasScanned(State state);

}
}

#endif
