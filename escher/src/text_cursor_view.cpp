#include <escher/blink_timer.h>
#include <escher/text_cursor_view.h>

namespace Escher {

OMG::GlobalBox<TextCursorView> TextCursorView::sharedTextCursor;

TextCursorView::~TextCursorView() { BlinkTimer::RegisterCursor(nullptr); }

void TextCursorView::drawRect(KDContext *ctx, KDRect rect) const {
  if (m_visible) {
    KDCoordinate height = bounds().height();
    ctx->fillRect(KDRect(0, 0, k_width, height), KDColorBlack);
  }
}

KDSize TextCursorView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_width, 0);
}

void TextCursorView::willMove() {
  /* Force the cursor to appears when its frame changes. This way, the user does
   * not lose sight of the cursor when moving around.  We need to expose a
   * callback since layoutSubviews is not called when the size is kept. */
  m_visible = true;
  // No need to mark rect as dirty since it will be moved
}

void TextCursorView::setBlinking(bool blinking, View *field) {
  if (blinking) {
    assert(field);
    m_field = field;
    BlinkTimer::RegisterCursor(const_cast<TextCursorView *>(this));
  } else {
    BlinkTimer::RegisterCursor(nullptr);
    m_field = nullptr;
  }
}

void TextCursorView::setVisible(bool visible) {
  if (visible == m_visible) {
    return;
  }
  m_visible = visible;
  if (visible) {
    markRectAsDirty(bounds());
  } else {
    // Redraw the part under the cursor
    m_field->markAbsoluteRectAsDirty(absoluteFrame());
  }
}

}  // namespace Escher
