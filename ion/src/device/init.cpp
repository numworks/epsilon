extern "C" {
#include "init.h"
#include <ion.h>
#include <assert.h>
#include "keyboard/keyboard.h"
#include "registers/registers.h"
}
#include "display.h"

void enable_fpu() {
  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BABDBFBJ.html
  //CPACR |= (0xF << 20); // Set the bits 20-23 to enable CP10 and CP11 coprocessors
  CPACR |= (
    REGISTER_FIELD_VALUE(CPACR_CP(10), CPACR_ACCESS_FULL)
    |
    REGISTER_FIELD_VALUE(CPACR_CP(11), CPACR_ACCESS_FULL)
    );
  // FIXME: The pipeline should be flushed at this point
}

void init_platform() {
  enable_fpu();
  init_keyboard();
  //ion_led_init();
  Ion::Screen::init();
}

void ion_sleep(long ms) {
  for (long i=0; i<1040*ms; i++) {
      __asm volatile("nop");
  }
}

void ion_reset() {
  AIRCR = AIRCR_VECTKEY_MASK | AIRCR_SYSRESETREQ;
}
