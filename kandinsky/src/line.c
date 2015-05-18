#include <kandinsky/line.h>
#include "framebuffer.h"

void KDDrawLine(KDPoint p1, KDPoint p2) {
  for (int i=0; i<10; i++) {
    PIXEL(i,i) = 0xFF;
  }
}
