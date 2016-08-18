#include <ion.h>
#include <assert.h>
#include "init.h"
#include "keyboard/keyboard.h"
#include "registers/registers.h"

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
  ion_led_init();
  ion_screen_init();
}

void ion_sleep() {
  // FIXME: Do something, and also move this function to its own file
}

void ion_reset() {
  AIRCR = AIRCR_VECTKEY_MASK | AIRCR_SYSRESETREQ;
}
