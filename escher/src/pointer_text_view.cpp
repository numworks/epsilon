#include <escher/pointer_text_view.h>
#include <assert.h>

PointerTextView::PointerTextView(const KDFont * font, const char * text, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  TextView(font, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_text(text)
{
}

void PointerTextView::setText(const char * text) {
  if (text != m_text) {
    m_text = text;
    markRectAsDirty(bounds());
  }
}
