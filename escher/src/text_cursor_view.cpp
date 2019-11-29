#include <escher/text_cursor_view.h>
#include <escher/palette.h>

void TextCursorView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  ctx->fillRect(KDRect(0, 0, 1, height), Palette::PrimaryText);
}

KDSize TextCursorView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_width, 0);
}

