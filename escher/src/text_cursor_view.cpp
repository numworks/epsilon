#include <escher/text_cursor_view.h>

void TextCursorView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  ctx->fillRect(KDRect(0, 0, 1, height), KDColorBlack);
}

KDSize TextCursorView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_width, 0);
}

