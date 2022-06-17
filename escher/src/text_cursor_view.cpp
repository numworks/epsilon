#include <escher/text_cursor_view.h>

namespace Escher {

void TextCursorView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  ctx->fillRect(KDRect(0, 0, k_width, height), m_visible ? KDColorBlack : KDColorWhite);
}

KDSize TextCursorView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_width, 0);
}

void TextCursorView::switchVisible() {
   m_visible = !m_visible;
   markRectAsDirty(bounds());
}

}
