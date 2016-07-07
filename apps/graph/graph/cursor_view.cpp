#include "cursor_view.h"

void CursorView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorRed);
}
