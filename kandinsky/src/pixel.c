#include <kandinsky/pixel.h>
#include <kandinsky/rect.h>
#include <assert.h>
#include <ion.h>
#include "private/drawing_area.h"

void KDSetPixel(KDPoint p, KDColor c) {
  KDPoint absolutePoint = KDPointTranslate(p, KDDrawingAreaOrigin);
  if (KDRectContains(KDDrawingAreaClippingRect, absolutePoint)) {
    ion_set_pixel(absolutePoint.x, absolutePoint.y, c);
  }
}
