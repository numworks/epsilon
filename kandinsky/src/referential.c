#include <kandinsky/referential.h>

static KDPoint sOrigin = {.x = 0, .y = 0};

void KDSetOrigin(KDPoint origin) {
  sOrigin = origin;
}

KDPoint KDGetOrigin() {
  return sOrigin;
}

KDColor * KDPixelAddress(KDPoint p) {
  KDPoint origin = KDGetOrigin();
  return (KDColor *)(FRAMEBUFFER_ADDRESS
      + FRAMEBUFFER_WIDTH*(p.y + origin.y)
      + (p.x + origin.x));
}

