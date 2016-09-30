#include "app_cell.h"
#include <assert.h>

namespace Home {

AppCell::AppCell() :
  ChildlessView()
{
}

void AppCell::drawRect(KDContext * ctx, KDRect rect) const {
  /*
  KDColor * pixels = (KDColor *)apps_picview_image_raw;
  assert(apps_picview_image_raw_len == bounds().width() * bounds().height() * sizeof(KDColor));
  ctx->fillRectWithPixels(bounds(), pixels, nullptr);
  */
}

}
