#include "app_cell.h"
#include <assert.h>

namespace Home {

AppCell::AppCell() :
  ChildlessView(),
  m_active(false)
{
}

void AppCell::setActive(bool active) {
  if (m_active != active) {
    m_active = active;
    markRectAsDirty(bounds());
  }
}

void AppCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor c = m_active ? KDColorRed : KDColorBlack;
  ctx->fillRect(KDRect(0,0, 20, 20), c);
  /*
  KDColor * pixels = (KDColor *)apps_picview_image_raw;
  assert(apps_picview_image_raw_len == bounds().width() * bounds().height() * sizeof(KDColor));
  ctx->fillRectWithPixels(bounds(), pixels, nullptr);
  */
}

}
