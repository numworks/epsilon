#include "pic_view.h"
#include <assert.h>

PicView::PicView() :
  View() {
}

#include "image.c"

void PicView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor * pixels = (KDColor *)apps_picview_image_raw;
  assert(apps_picview_image_raw_len == bounds().width() * bounds().height() * sizeof(KDColor));
  ctx->fillRectWithPixels(bounds(), pixels, nullptr);
}
