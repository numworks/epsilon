#include <kandinsky/line.h>
#include "framebuffer.h"

void KDDrawLine(kdpoint_t * p1, kdpoint_t * p2) {
  for (int i=0; i<10; i++) {
    PIXEL(i,i) = 0xFF;
  }
}
