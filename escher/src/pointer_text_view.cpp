#include <assert.h>
#include <escher/pointer_text_view.h>

namespace Escher {

PointerTextView::PointerTextView(const char* text, KDGlyph::Format format)
    : TextView(format), m_text(text) {}

void PointerTextView::setText(const char* text) {
  if (text != m_text) {
    m_text = text;
    markRectAsDirty(bounds());
  }
}

}  // namespace Escher
