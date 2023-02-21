#include <assert.h>
#include <escher/pointer_text_view.h>

namespace Escher {

PointerTextView::PointerTextView(KDFont::Size font, const char* text,
                                 float horizontalAlignment,
                                 float verticalAlignment, KDColor textColor,
                                 KDColor backgroundColor)
    : TextView(font, horizontalAlignment, verticalAlignment, textColor,
               backgroundColor),
      m_text(text) {}

void PointerTextView::setText(const char* text) {
  if (text != m_text) {
    m_text = text;
    markRectAsDirty(bounds());
  }
}

}  // namespace Escher
