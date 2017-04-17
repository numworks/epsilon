#include <escher/message_text_view.h>
#include <assert.h>

MessageTextView::MessageTextView(KDText::FontSize size, I18n::Message message, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  TextView(size, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_message(message)
{
}

const char * MessageTextView::text() const {
  return I18n::translate(m_message);
}

void MessageTextView::setText(const char * text) {
  assert(false);
}

void MessageTextView::setMessage(I18n::Message message) {
  if (message != m_message) {
    m_message = message;
    markRectAsDirty(bounds());
  }
}

KDSize MessageTextView::minimalSizeForOptimalDisplay() const  {
  KDCoordinate width = 0;
  for (int l = 0; l < I18n::numberOfLanguages()+1; l++) {
    KDCoordinate newWidth = KDText::stringSize(I18n::translate(m_message, (I18n::Language)l), m_fontSize).width();
    width = width > newWidth ?  width : newWidth;
  }
  return KDSize(width, KDText::stringSize(text(), m_fontSize).height());
}
