#include <escher/buffer_text_view.h>
#include <string.h>
#include <assert.h>

namespace Escher {

BufferTextView::BufferTextView(const KDFont * font, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor, int maxDisplayedTextLength) :
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

void BufferTextView::appendText(const char * text) {
  size_t previousTextLength = strlen(m_buffer);
  if (previousTextLength < m_maxDisplayedTextLength) {
    strlcpy(&m_buffer[previousTextLength], text, m_maxDisplayedTextLength + 1 - previousTextLength);
  }
}

}
