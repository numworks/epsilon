#include <kandinsky/drawing_area.h>
#include <ion.h>

KDRect KDDrawingArea = {
  .x = 0,
  .y = 0,
  .width = ION_SCREEN_WIDTH,
  .height = ION_SCREEN_HEIGHT
};

void KDSetDrawingArea(KDRect rect) {
  KDDrawingArea = rect;
}
