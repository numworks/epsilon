#include <kandinsky/pixel.h>
#include <assert.h>
#include <ion.h>
#include "private/drawing_area.h"

void KDSetPixel(KDPoint p, KDColor c) {
  if (p.x >= 0 && p.x < KDDrawingArea.width &&
      p.y >= 0 && p.y < KDDrawingArea.height) {
    ion_set_pixel(p.x+KDDrawingArea.x, p.y+KDDrawingArea.y, c);
  }
}
