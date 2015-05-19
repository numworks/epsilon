#include <kandinsky/line.h>
#include <framebuffer.h>

void KDDrawLine(KDPoint p1, KDPoint p2) {
  for (KDCoordinate x = p1.x; x<p2.x; x++) {
    PIXEL(x, p1.y) = 0xC7;
  }
}
