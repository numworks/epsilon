#include "vertical_cursor_view.h"

namespace Sequence {

void VerticalCursorView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  ctx->fillRect(KDRect(0, 0, 1, height), KDColorBlack);
}

}
