#include <escher/blink_timer.h>
#include <escher/text_cursor_view.h>

namespace Escher {

TextCursorView::~TextCursorView() {
  BlinkTimer::registerCursor(nullptr);
}

void TextCursorView::drawRect(KDContext * ctx, KDRect rect) const {
  BlinkTimer::registerCursor(const_cast<TextCursorView *>(this));
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
