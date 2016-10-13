#include "cursor_view.h"

namespace Graph {

void CursorView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorRed);
}

}
