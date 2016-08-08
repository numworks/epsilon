#include "pic_view.h"
#include <assert.h>

PicView::PicView() :
  ChildlessView() {
}

#include "image.c"

void PicView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor * pixels = (KDColor *)apps_picview_image_raw;
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  assert(apps_picview_image_raw_len == width*height*sizeof(KDColor));
  for (int j=0; j<height; j++) {
    for (int i=0; i<width; i++) {
      ctx->setPixel(KDPoint(i,j), pixels[j*width+i]);
    }
  }
}
