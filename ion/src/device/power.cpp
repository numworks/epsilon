#include <ion.h>
#include "regs/regs.h"
#include "keyboard.h"
#include "led.h"
#include "display.h"

void Ion::Power::suspend() {
  LED::Device::suspend();
  Display::Device::suspend();

  CM4.SCR()->setSLEEPDEEP(true);
  Keyboard::Device::generateWakeUpEventForKey(Ion::Keyboard::Key::B2);
  msleep(300);
  asm("wfe");

  Display::Device::resume();
  LED::Device::resume();
}
