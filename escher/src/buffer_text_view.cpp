#include <escher/buffer_text_view.h>
#include <string.h>
#include <assert.h>

BufferTextView::BufferTextView(KDText::FontSize size, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  TextView(size, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
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
