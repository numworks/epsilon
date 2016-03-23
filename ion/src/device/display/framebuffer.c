#include <ion.h>
#include <assert.h>
#include "framebuffer.h"

#define BIT_MASK(high, low) ((((uint32_t)1<<((high)-(low)+1))-1)<<(low))
#define BIT_VALUE(value, high, low) (((value)<<(low))&BIT_MASK(high, low))
#define BIT_SET(initial, value, high, low) ((initial&~BIT_MASK(high,low))|(BIT_VALUE(value,high,low)&BIT_MASK(high,low)))


/* We're using a pixel format imposed to us by the stupid st7586. We use a four
 * color framebuffer, so that's 2 bits per pixels. Unfortunately we cannot pack
 * those pixels and have to resort to two pixels per byte using the following
 * format : AAxBBxxx, where AA and BB are the bits for two contiguous pixels. */

void ion_set_pixel(uint16_t x, uint16_t y, uint8_t color) {
  assert(x <= FRAMEBUFFER_WIDTH);
  assert(y <= FRAMEBUFFER_HEIGHT);

  char * byte = FRAMEBUFFER_ADDRESS + ((y*FRAMEBUFFER_WIDTH)+x)*FRAMEBUFFER_BITS_PER_PIXEL/8;
  assert(byte <= &_framebuffer_end);

  char mask, value;
  if (x & 1) {
    mask = BIT_MASK(4,3);
    value = BIT_VALUE(color,4,3);
  } else {
    mask = BIT_MASK(7,6);
    value = BIT_VALUE(color,7,6);
  }
  *byte = (*byte & ~mask) | (value & mask);
}
