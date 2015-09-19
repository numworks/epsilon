#include "framebuffer.h"
#include <ion.h>
#include <assert.h>

void ion_set_pixel(uint8_t x, uint8_t y, uint8_t color) {
  assert(x <= FRAMEBUFFER_WIDTH);
  assert(y <= FRAMEBUFFER_HEIGHT);

  char * byte = FRAMEBUFFER_ADDRESS + ((y*FRAMEBUFFER_WIDTH)+x);
  *byte = color;
}
