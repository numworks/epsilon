#include <ion.h>
#include <assert.h>
#include "init.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

#define CPACR (*(volatile uint32_t *)(0xE000ED88))

void enable_fpu() {
  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BABDBFBJ.html
  CPACR |= (0xF << 20); // Set the bits 20-23 to enable CP10 and CP11 coprocessors
  // FIXME: The pipeline should be flushed at this point
}

void init_platform() {
  enable_fpu();
  init_keyboard();
  init_display();
}

void ion_sleep() {
  // FIXME: Do something, and also move this function to its own file
}
