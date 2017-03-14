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
