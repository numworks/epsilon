#include <kandinsky/pixel.h>
#include <kandinsky/rect.h>
#include <kandinsky/context.h>
#include <assert.h>
#include <ion.h>

void KDSetPixel(KDPoint p, KDColor c) {
  KDPoint absolutePoint = KDPointTranslate(p, KDCurrentContext->origin);
  if (KDRectContains(KDCurrentContext->clippingRect, absolutePoint)) {
    KDCurrentContext->setPixel(absolutePoint.x, absolutePoint.y, c);
  }
}
