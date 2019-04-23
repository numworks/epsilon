#include <escher/text_view.h>

void TextView::setBackgroundColor(KDColor backgroundColor) {
  if (m_backgroundColor != backgroundColor) {
    m_backgroundColor = backgroundColor;
    markRectAsDirty(bounds());
  }
}

void TextView::setTextColor(KDColor textColor) {
  if (m_textColor != textColor) {
    m_textColor = textColor;
    markRectAsDirty(bounds());
  }
}

void TextView::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_horizontalAlignment = horizontalAlignment;
  m_verticalAlignment = verticalAlignment;
  markRectAsDirty(bounds());
}

void TextView::setFont(const KDFont * font) {
  m_font = font;
  markRectAsDirty(bounds());
}

KDSize TextView::minimalSizeForOptimalDisplay() const  {
  if (text() == nullptr) {
    return KDSize(0,0);
  }
  return m_font->stringSize(text());
}

void TextView::drawRect(KDContext * ctx, KDRect rect) const {
  if (text() == nullptr) {
    return;
  }
  KDSize textSize = m_font->stringSize(text());
  KDPoint origin(
      m_horizontalAlignment * (m_frame.width() - textSize.width()),
      m_verticalAlignment * (m_frame.height() - textSize.height()));
  ctx->fillRect(bounds(), m_backgroundColor);
  ctx->drawString(text(), origin, m_font, m_textColor, m_backgroundColor);
}
