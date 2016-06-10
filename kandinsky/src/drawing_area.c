#include <kandinsky/drawing_area.h>

KDRect KDDrawingArea = {
  .x = 0,
  .y = 0,
  .width = KDCoordinateMax,
  .height = KDCoordinateMax
};

void KDSetDrawingArea(KDRect rect) {
  KDDrawingArea = rect;
}
