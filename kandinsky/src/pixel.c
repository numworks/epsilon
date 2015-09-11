#include <kandinsky/pixel.h>
#include <assert.h>
#include <ion.h>

static KDPoint sOrigin = {.x = 0, .y = 0};

void KDSetOrigin(KDPoint origin) {
  sOrigin = origin;
}

KDPoint KDGetOrigin() {
  return sOrigin;
}

void KDSetPixel(KDPoint p, KDColor c) {
  ion_set_pixel(p.x+sOrigin.x, p.y+sOrigin.y, c);
}
