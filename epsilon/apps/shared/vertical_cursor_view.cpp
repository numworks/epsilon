#include "vertical_cursor_view.h"

#include <escher/palette.h>

namespace Shared {

void VerticalCursorView::drawRect(KDContext* ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  ctx->fillRect(KDRect(0, 0, 1, height), Escher::Palette::Black);
}

KDSize VerticalCursorView::minimalSizeForOptimalDisplay() const {
  return KDSize(1, 0);
}

}  // namespace Shared
