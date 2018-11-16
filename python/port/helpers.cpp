#include "helpers.h"
#include <ion.h>
extern "C" {
#include "mphalport.h"
}

void micropython_port_vm_hook_loop() {
  /* This function is called very frequently by the MicroPython engine. We grab
   * this opportunity to interrupt execution and/or refresh the display on
   * platforms that need it. */

  /* Doing too many things here slows down Python execution quite a lot. So we
   * only do things once in a while and return as soon as possible otherwise. */
  static int c = 0;

  c = (c + 1) % 20000;
  if (c != 0) {
    return;
  }

  /* Check if the user asked for an interruption from the keyboard */
  if (micropython_port_should_interrupt()) {
    mp_keyboard_interrupt();
  }
}

bool micropython_port_should_interrupt() {
  Ion::Keyboard::State scan = Ion::Keyboard::scan();
  Ion::Keyboard::Key interruptKey = static_cast<Ion::Keyboard::Key>(mp_interrupt_char);
  return scan.keyDown(interruptKey);
}

void micropython_port_interruptible_msleep(uint32_t delay) {
  uint32_t start = Ion::Timing::millis();
  while (Ion::Timing::millis() - start < delay && !micropython_port_should_interrupt()) {
    Ion::Timing::msleep(1);
  }
}
