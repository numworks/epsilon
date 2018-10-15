#include <escher/buffer_text_view.h>
#include <string.h>
#include <assert.h>

BufferTextView::BufferTextView(const KDFont * font, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  TextView(font, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_buffer()
{
}

const char * BufferTextView::text() const {
  return m_buffer;
}

void BufferTextView::setText(const char * text) {
  assert(strlen(text) < sizeof(m_buffer));
  strlcpy(m_buffer, text, sizeof(m_buffer));
  markRectAsDirty(bounds());
}

void BufferTextView::appendText(const char * text) {
  size_t previousTextLength = strlen(m_buffer);
  size_t argTextLength = strlen(text);
  if (previousTextLength + argTextLength + 1 < k_maxNumberOfChar) {
    strlcpy(&m_buffer[previousTextLength], text, k_maxNumberOfChar - previousTextLength);
  }
}
