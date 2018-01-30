#include <escher/pointer_text_view.h>
#include <assert.h>

PointerTextView::PointerTextView(KDText::FontSize size, const char * text, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  TextView(size, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_text(text)
{
}

void PointerTextView::setText(const char * text) {
  if (text != m_text) {
    m_text = text;
    markRectAsDirty(bounds());
  }
}

KDSize PointerTextView::minimalSizeForOptimalDisplay() const  {
  return KDText::stringSize(text(), m_fontSize);
}
