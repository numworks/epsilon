#include <kandinsky/pixel.h>
#include <assert.h>
#include <ion.h>

static KDRect sDrawingArea = {
  .x = 0,
  .y = 0,
  .width = KDCoordinateMax,
  .height = KDCoordinateMax
};

void KDSetDrawingArea(KDRect rect) {
  sDrawingArea = rect;
}

void KDSetPixel(KDPoint p, KDColor c) {
  if (p.x >= 0 && p.x < sDrawingArea.width &&
      p.y >= 0 && p.y < sDrawingArea.height) {
    ion_set_pixel(p.x+sDrawingArea.x, p.y+sDrawingArea.y, c);
  }
}
