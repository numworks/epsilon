#include "cursor_view.h"

namespace Shared {

void CursorView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  ctx->fillRect(KDRect(width/2, 0, 1, height), KDColorBlack);
  ctx->fillRect(KDRect(0, height/2, width, 1), KDColorBlack);
}

KDSize CursorView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_size, k_size);
}

}
