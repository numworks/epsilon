#include "keyboard_init.h"

namespace Ion {
namespace Device {
namespace Keyboard {

void init() {
  initGPIO();
}

void shutdown() {
  shutdownGPIO();
}

void keyboardWasScanned(State) {}

void stopPollTimer() {}

}
}
}
