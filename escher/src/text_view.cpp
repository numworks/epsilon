#include <escher/text_view.h>

TextView::TextView(KDText::FontSize size, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  View(),
  m_horizontalAlignment(horizontalAlignment),
  m_verticalAlignment(verticalAlignment),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor),
  m_fontSize(size)
{
}

void TextView::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  markRectAsDirty(bounds());
}

void TextView::setTextColor(KDColor textColor) {
  m_textColor = textColor;
  markRectAsDirty(bounds());
}

void TextView::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_horizontalAlignment = horizontalAlignment;
  m_verticalAlignment = verticalAlignment;
  markRectAsDirty(bounds());
}

KDSize TextView::minimalSizeForOptimalDisplay() {
  return KDText::stringSize(text(), m_fontSize);
}

void TextView::drawRect(KDContext * ctx, KDRect rect) const {
  if (text() == nullptr) {
    return;
  }
  KDSize textSize = KDText::stringSize(text(), m_fontSize);
  KDPoint origin(m_horizontalAlignment*(m_frame.width() - textSize.width()),
    m_verticalAlignment*(m_frame.height() - textSize.height()));
  ctx->fillRect(bounds(), m_backgroundColor);
  ctx->drawString(text(), m_fontSize, origin, m_textColor, m_backgroundColor);
}

#if ESCHER_VIEW_LOGGING
const char * TextView::className() const {
  return "TextView";
}
#endif
