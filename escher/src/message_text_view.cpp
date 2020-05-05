#include <escher/message_text_view.h>
#include <assert.h>
#include <apps/i18n.h>

MessageTextView::MessageTextView(const KDFont * font, I18n::Message message, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  TextView(font, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_message(message)
{
}

const char * MessageTextView::text() const {
  if (m_message != I18n::Message::Default && m_text == nullptr)
    return I18n::translate(m_message);
  if (m_text != nullptr && m_message == I18n::Message::Default)
    return m_text;
  assert(false);
  return nullptr;
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
