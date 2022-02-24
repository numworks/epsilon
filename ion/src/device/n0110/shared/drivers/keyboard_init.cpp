#include "keyboard_init.h"

namespace Ion {
namespace Keyboard {

void keyboardWasScanned(State) {}

}
}

namespace Ion {
namespace Device {
namespace Keyboard {

void init() {
  initGPIO();
}

void shutdown() {
  shutdownGPIO();
}

void stopPollTimer() {}

}
}
}
