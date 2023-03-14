#include <assert.h>
#include <escher/buffer_text_view.h>
#include <escher/i18n.h>
#include <poincare/print.h>
#include <string.h>

namespace Escher {

BufferTextView::BufferTextView(KDGlyph::Format format,
                               size_t maxDisplayedTextLength)
    : TextView(format), m_maxDisplayedTextLength(maxDisplayedTextLength) {
  assert(m_maxDisplayedTextLength < k_maxNumberOfChar &&
         m_maxDisplayedTextLength >= 0);
  m_buffer[0] = 0;
}

const char* BufferTextView::text() const { return m_buffer; }

void BufferTextView::setText(const char* text) {
  assert(m_maxDisplayedTextLength < sizeof(m_buffer));
  strlcpy(m_buffer, text, m_maxDisplayedTextLength + 1);
  markRectAsDirty(bounds());
}

void BufferTextView::setMessageWithPlaceholders(I18n::Message message, ...) {
  va_list args;
  va_start(args, message);
  privateSetMessageWithPlaceholders(message, args);
  va_end(args);
}

void BufferTextView::privateSetMessageWithPlaceholders(I18n::Message message,
                                                       va_list args) {
  char tempBuffer[k_maxNumberOfChar];
  Poincare::Print::PrivateCustomPrintf(tempBuffer, m_maxDisplayedTextLength + 1,
                                       I18n::translate(message), args);
  setText(tempBuffer);
}

void BufferTextView::appendText(const char* text) {
  size_t previousTextLength = strlen(m_buffer);
  if (previousTextLength < m_maxDisplayedTextLength) {
    strlcpy(&m_buffer[previousTextLength], text,
            m_maxDisplayedTextLength + 1 - previousTextLength);
  }
}

KDSize BufferTextView::minimalSizeForOptimalDisplay() const {
  return KDFont::Font(font())->stringSize(text());
}

}  // namespace Escher
