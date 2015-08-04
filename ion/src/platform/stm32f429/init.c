#include <ion.h>
#include <assert.h>
#include "display.h"
#include "init_kbd.h"

extern char _framebuffer_start;
extern char _framebuffer_end;

void ion_init() {
  ion_framebuffer_address = &_framebuffer_start;
  ion_framebuffer_width = 240;
  ion_framebuffer_height = 320;
  ion_framebuffer_bits_per_pixel = 8;

#if DEBUG
  char * computed_framebuffer_end = ion_framebuffer_address + (ion_framebuffer_width*ion_framebuffer_height*ion_framebuffer_bits_per_pixel)/8;
  assert(&_framebuffer_end == computed_framebuffer_end);
#endif

  init_kbd();
  init_display();
}
