#include <kandinsky/drawing_area.h>
#include <ion.h>

KDPoint KDDrawingAreaOrigin = {
  .x = 0,
  .y = 0
};

KDRect KDDrawingAreaClippingRect = {
  .x = 0,
  .y = 0,
  .width = ION_SCREEN_WIDTH,
  .height = ION_SCREEN_HEIGHT
};

void KDSetDrawingArea(KDPoint origin, KDRect clippingRect) {
  KDDrawingAreaOrigin = origin;
  KDDrawingAreaClippingRect = clippingRect;
}
