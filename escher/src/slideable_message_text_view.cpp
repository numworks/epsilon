#include <escher/slideable_message_text_view.h>

SlideableMessageTextView::SlideableMessageTextView(const KDFont * font, I18n::Message message, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  MessageTextView(font, message, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_textOffset(0)
{

}

void SlideableMessageTextView::willStartAnimation() {
  m_textOffset = 0;
  m_goingLeft = true;
  m_paused = true;
}

void SlideableMessageTextView::didStopAnimation() {
  m_textOffset = 0;
}

void SlideableMessageTextView::animate() {
  if (m_paused) {
    m_paused = false;
    return;
  }

  if (text() == nullptr) {
    return;
  }

  KDSize textSize = m_font->stringSize(text());

  if (textSize.width() <= bounds().width()) {
    return;
  }

  KDCoordinate glyphWidth = m_font->glyphSize().width();
  m_textOffset += glyphWidth * (m_goingLeft ? -1 : 1);

  if (m_goingLeft && textSize.width() + m_textOffset < bounds().width()) {
    m_goingLeft = false;
    m_textOffset = bounds().width() - textSize.width();
    m_paused = true;
  } else if (!m_goingLeft && m_textOffset > 0) {
    m_goingLeft = true;
    m_textOffset = 0;
    m_paused = true;
  }

  markRectAsDirty(bounds());
}

void SlideableMessageTextView::drawRect(KDContext * ctx, KDRect rect) const {
  if (text() == nullptr) {
    return;
  }
  KDSize textSize = m_font->stringSize(text());
  KDPoint origin(
      m_horizontalAlignment * (m_frame.width() - textSize.width()) + m_textOffset,
      m_verticalAlignment * (m_frame.height() - textSize.height()));
  ctx->fillRect(bounds(), m_backgroundColor);
  ctx->drawString(text(), origin, m_font, m_textColor, m_backgroundColor);
}
