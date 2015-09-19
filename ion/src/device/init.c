#include <ion.h>
#include <assert.h>
#include "display.h"
#include "keyboard.h"

//#extern char _framebuffer_end;

void ion_init() {
  /*
#if DEBUG
  char * computed_framebuffer_end = ION_FRAMEBUFFER_ADDRESS + (ION_FRAMEBUFFER_WIDTH*ION_FRAMEBUFFER_HEIGHT*ION_FRAMEBUFFER_BITS_PER_PIXEL)/8;
  assert(&_framebuffer_end == computed_framebuffer_end);
#endif
*/

  init_keyboard();
  init_display();
}

void ion_sleep() {
  // FIXME
}
