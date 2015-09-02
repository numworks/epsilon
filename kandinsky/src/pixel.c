#include <kandinsky/pixel.h>
#include <kandinsky/config.h>
#include <assert.h>

static KDPoint sOrigin = {.x = 0, .y = 0};

#define BIT_MASK(high, low) ((((uint32_t)1<<((high)-(low)+1))-1)<<(low))
#define BIT_VALUE(value, high, low) (((value)<<(low))&BIT_MASK(high, low))

static inline void set_color(KDColor * address, int8_t high_bit, int8_t low_bit, KDColor value) {
  assert(high_bit-low_bit+1 <= 8*sizeof(KDColor)); // Interval too large
  assert(low_bit >= 0); //  Interval starts too early
  assert(low_bit < 8*sizeof(KDColor)); // Interval ends too late
  KDColor mask = ((((KDColor)1<<(high_bit-low_bit+1))-1)<<low_bit);
  assert((void *)address >= KD_FRAMEBUFFER_ADDRESS);
  assert((void *)address < (KD_FRAMEBUFFER_ADDRESS+(KD_FRAMEBUFFER_WIDTH*KD_FRAMEBUFFER_HEIGHT*KD_BITS_PER_PIXEL)/8));
  *address = (*address & ~mask) | (value & mask);
}

void KDSetOrigin(KDPoint origin) {
  sOrigin = origin;
}

KDPoint KDGetOrigin() {
  return sOrigin;
}

void KDSetPixel(KDPoint p, KDColor c) {
  KDPoint t = {
    .x = p.x + sOrigin.x,
    .y = p.y + sOrigin.y
  };
  assert(t.x >= 0);
  assert(t.x < KD_FRAMEBUFFER_WIDTH);
  assert(t.y >= 0);
  assert(t.y < KD_FRAMEBUFFER_HEIGHT);
  int pixels_offset = t.y*(KD_FRAMEBUFFER_WIDTH) + t.x;
  int bits_offset = (KD_BITS_PER_PIXEL)*pixels_offset;
  const uint8_t color_bitsize = 8*sizeof(KDColor);
  // Notice: the "const" here is rather important. Indeed, it hints the compiler
  // for the following "modulo", which helps us avoid idivmod.
  int color_offset = bits_offset/color_bitsize;
  int8_t low_bit = bits_offset % color_bitsize;
  int8_t high_bit = low_bit + KD_BITS_PER_PIXEL - 1;
  set_color((KDColor *)(KD_FRAMEBUFFER_ADDRESS+color_offset), high_bit, low_bit, c);
}
