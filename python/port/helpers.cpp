#include "helpers.h"
#include <ion.h>
extern "C" {
#include "mphalport.h"
}

bool micropython_port_vm_hook_loop() {
  /* This function is called very frequently by the MicroPython engine. We grab
   * this opportunity to interrupt execution and/or refresh the display on
   * platforms that need it. */

  /* Doing too many things here slows down Python execution quite a lot. So we
   * only do things once in a while and return as soon as possible otherwise. */
  static int c = 0;

  c = (c + 1) % 20000;
  if (c != 0) {
    return false;
  }

  // Check if the user asked for an interruption from the keyboard
  return micropython_port_interrupt_if_needed();
}

bool micropython_port_interruptible_msleep(uint32_t delay) {
  /* We don't use millis because the systick drifts when changing the HCLK
   * frequency. */
  constexpr uint32_t interruptionCheckDelay = 100;
  const uint32_t numberOfInterruptionChecks = delay / interruptionCheckDelay;
  uint32_t remainingInterruptionChecks = numberOfInterruptionChecks;
  while (remainingInterruptionChecks > 0) {
    // We assume the time taken by the interruption check is insignificant
    if (micropython_port_interrupt_if_needed()) {
      return true;
    }
    remainingInterruptionChecks--;
    Ion::Timing::msleep(interruptionCheckDelay);
  }
  Ion::Timing::msleep(delay - numberOfInterruptionChecks * interruptionCheckDelay);
  return false;
}

bool micropython_port_interrupt_if_needed() {
  Ion::Keyboard::State scan = Ion::Keyboard::scan();
  Ion::Keyboard::Key interruptKey = static_cast<Ion::Keyboard::Key>(mp_interrupt_char);
  if (scan.keyDown(interruptKey)) {
    mp_keyboard_interrupt();
    return true;
  }
  return false;
}

int micropython_port_random() {
  return Ion::random();
}
