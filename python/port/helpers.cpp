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
  c++;
  if (c % 20000 != 0) {
    return;
  }
  c = 0;

  /* Check if the user asked for an interruption from the keyboard */
  Ion::Keyboard::State scan = Ion::Keyboard::scan();
  if (scan.keyDown(static_cast<Ion::Keyboard::Key>(mp_interrupt_char))) {
    mp_keyboard_interrupt();
  }
}

