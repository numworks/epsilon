#include "helpers.h"
#include <ion.h>
extern "C" {
#include "mphalport.h"
}

void micropython_port_should_interrupt() {
  static int c = 0;
  c++;
  if (c%20000 != 0) {
    return;
  }
  c = 0;
  Ion::Keyboard::State scan = Ion::Keyboard::scan();
  if (scan.keyDown((Ion::Keyboard::Key)mp_interrupt_char)) {
    mp_keyboard_interrupt();
  }
}

