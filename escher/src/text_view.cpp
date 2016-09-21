#include <escher/text_view.h>

TextView::TextView() : TextView(nullptr, 0.0f, 0.0f, KDColorBlack, KDColorWhite)
{
}

TextView::TextView(const char * text, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  ChildlessView(),
  m_text(text),
  m_horizontalAlignment(horizontalAlignment),
  m_verticalAlignment(verticalAlignment),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor)
{
}

void TextView::setText(const char * text) {
  m_text = text;
}

void TextView::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  markRectAsDirty(bounds());
}

void TextView::setTextColor(KDColor textColor) {
  m_textColor = textColor;
  markRectAsDirty(bounds());
}

void TextView::drawRect(KDContext * ctx, KDRect rect) const {
  KDSize textSize = KDText::stringSize(m_text);
  KDPoint origin = {
    (KDCoordinate)(m_horizontalAlignment*(m_frame.width() - textSize.width())),
    (KDCoordinate)(m_verticalAlignment*(m_frame.height() - textSize.height()))
  };
  ctx->drawString(m_text, origin, m_textColor, m_backgroundColor);
}

#if ESCHER_VIEW_LOGGING
const char * TextView::className() const {
  return "TextView";
}
#endif
