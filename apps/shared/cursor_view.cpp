#include "cursor_view.h"

namespace Shared {

void CursorView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  ctx->fillRect(KDRect((width-1)/2, 0, 1, height), Palette::PrimaryText);
  ctx->fillRect(KDRect(0, (height-1)/2, width, 1), Palette::PrimaryText);
}

KDSize CursorView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_size, k_size);
}

}
