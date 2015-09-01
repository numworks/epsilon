#include <kandinsky/line.h>
#include <kandinsky/pixel.h>

void KDDrawLine(KDPoint p1, KDPoint p2) { //, KDColor c) {
  for (KDCoordinate x = p1.x; x<p2.x; x++) {
    KDSetPixel(KDPOINT(x, p1.y), 0xFF /* c */);
  }
}
