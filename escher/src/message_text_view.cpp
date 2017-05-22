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
  return KDText::stringSize(text(), m_fontSize);
}
