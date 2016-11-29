#include <escher/buffer_text_view.h>
#include <string.h>
#include <assert.h>

BufferTextView::BufferTextView() :
  TextView(0.5f, 0.5f, KDColorBlack, KDColorWhite)
{
}

BufferTextView::BufferTextView(float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  TextView(horizontalAlignment, verticalAlignment, textColor, backgroundColor)
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
