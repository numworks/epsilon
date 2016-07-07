#include <kandinsky/pixel.h>
#include <kandinsky/rect.h>
#include <kandinsky/context.h>
#include <assert.h>
#include <ion.h>

void KDSetPixel(KDPoint p, KDColor c) {
  KDPoint absolutePoint = KDPointTranslate(p, KDCurrentContext->origin);
  if (KDRectContains(KDCurrentContext->clippingRect, absolutePoint)) {
    KDCurrentContext->io.pushRect((KDRect){.origin = absolutePoint, .width = 1, .height = 1}, &c);
  }
}

KDColor KDGetPixel(KDPoint p) {
  KDPoint absolutePoint = KDPointTranslate(p, KDCurrentContext->origin);
  if (KDRectContains(KDCurrentContext->clippingRect, absolutePoint)) {
    KDColor result;
    KDCurrentContext->io.pullRect((KDRect){.origin = absolutePoint, .width = 1, .height = 1}, &result);
    return result;
  }
  return KDColorBlack;
}
/*
void KDSetAbsolutePixelDirect(KDPoint p, KDColor c) {
  KDCurrentContext->io.setWorkingArea((KDRect){.origin = p, .width = 1, .height = 1});
  KDCurrentContext->io.pushPixels(&c, 1);
}

KDColor KDGetAbsolutePixelDirect(KDPoint p) {
  KDColor result;
  KDCurrentContext->io.setWorkingArea((KDRect){.origin = p, .width = 1, .height = 1});
  KDCurrentContext->io.pullPixels(&result, 1);
  return result;
}
*/
