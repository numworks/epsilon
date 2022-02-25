#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <string.h>
#include <poincare/print.h>
#include <assert.h>

namespace Escher {

BufferTextView::BufferTextView(const KDFont * font, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor, size_t maxDisplayedTextLength) :
  TextView(font, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_buffer(),
  m_maxDisplayedTextLength(maxDisplayedTextLength)
{
  assert(m_maxDisplayedTextLength < k_maxNumberOfChar && m_maxDisplayedTextLength >= 0);
}

const char * BufferTextView::text() const {
  return m_buffer;
}

void BufferTextView::setText(const char * text) {
  assert(m_maxDisplayedTextLength < sizeof(m_buffer));
  strlcpy(m_buffer, text, m_maxDisplayedTextLength + 1);
  markRectAsDirty(bounds());
}

// This method only combine a message and 1 string because it is the most common case.
// It could be rewritten to take a va_list to combin a message with more strings.
void BufferTextView::setMessageAndStringAsText(I18n::Message message, const char * string) {
  char tempBuffer[m_maxDisplayedTextLength + 1];
  Poincare::Print::customPrintf(tempBuffer, m_maxDisplayedTextLength + 1, I18n::translate(message), string);
  setText(tempBuffer);
}

void BufferTextView::appendText(const char * text) {
  size_t previousTextLength = strlen(m_buffer);
  if (previousTextLength < m_maxDisplayedTextLength) {
    strlcpy(&m_buffer[previousTextLength], text, m_maxDisplayedTextLength + 1 - previousTextLength);
  }
}

}
