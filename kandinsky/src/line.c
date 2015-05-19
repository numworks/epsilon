#include <kandinsky/line.h>
#include <kandinsky/referential.h>

void KDDrawLine(KDPoint p1, KDPoint p2) {
  for (KDCoordinate x = p1.x; x<p2.x; x++) {
    COLOR(KDPOINT(x, p1.y)) = 0xFF;
  }
}
