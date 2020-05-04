#include <escher/message_text_view.h>
#include <assert.h>

MessageTextView::MessageTextView(const KDFont * font, I18n::Message message, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  TextView(font, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_message(message)
{
}

const char * MessageTextView::text() const {
  if (m_text)
    return m_text;
  return I18n::translate(m_message);
}

void MessageTextView::setText(const char * text) {
  m_text = text;
}

void MessageTextView::setMessage(I18n::Message message) {
  if (message != m_message) {
    m_message = message;
    markRectAsDirty(bounds());
  }
}

KDSize MessageTextView::minimalSizeForOptimalDisplay() const  {
  return m_font->stringSize(text());
}
