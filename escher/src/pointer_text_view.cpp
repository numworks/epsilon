#include <escher/pointer_text_view.h>
#include <assert.h>

PointerTextView::PointerTextView(KDText::FontSize size, I18n::Message message, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  TextView(size, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_message(message)
{
}

const char * PointerTextView::text() const {
  return I18n::translate(m_message);
}

void PointerTextView::setText(const char * text) {
  assert(false);
}

void PointerTextView::setMessage(I18n::Message message) {
  m_message = message;
  markRectAsDirty(bounds());
}

KDSize PointerTextView::minimalSizeForOptimalDisplay() const  {
  KDCoordinate width = 0;
  for (int l = 0; l < I18n::numberOfLanguages(); l++) {
    KDCoordinate newWidth = KDText::stringSize(I18n::translate(m_message, (I18n::Language)l), m_fontSize).width();
    width = width > newWidth ?  width : newWidth;
    l++;
  }
  return KDSize(width, KDText::stringSize(text(), m_fontSize).height());
}
