#include <ion/keyboard.h>
#include "platform.h"
#include "main.h"

#include <3ds.h>

void IonSimulatorKeyboardKeyDown(int keyNumber) {
}

void IonSimulatorKeyboardKeyUp(int keyNumber) {
}

namespace Ion {
namespace Keyboard {

State scan() {
	hidScanInput();
  Simulator::Main::refresh();
  return 0;
}

}
}
