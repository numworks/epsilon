#include <escher/pointer_text_view.h>

PointerTextView::PointerTextView(const char * text, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  TextView(horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_textPointer(text)
{
}

const char * PointerTextView::text() const {
  return m_textPointer;
}

void PointerTextView::setText(const char * text) {
  m_textPointer = text;
  markRectAsDirty(bounds());
}