#include <ion/power.h>
#include <ion/display.h>
#include "keyboard.h"

void Ion::Power::suspend() {
  Display::setBacklightIntensity(0);
  Keyboard::Device::generateWakeUpEventForKey(Ion::Keyboard::Key::J1);
  asm("wfe");
  Display::setBacklightIntensity(0xFF);
}
