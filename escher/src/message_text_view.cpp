#include <assert.h>
#include <escher/message_text_view.h>

namespace Escher {

const char* MessageTextView::text() const { return I18n::translate(m_message); }

void MessageTextView::setText(const char* text) { assert(false); }

void MessageTextView::setMessage(I18n::Message message) {
  if (message != m_message) {
    m_message = message;
    markRectAsDirty(bounds());
  }
}

KDSize MessageTextView::minimalSizeForOptimalDisplay() const {
  return KDFont::Font(font())->stringSize(text());
}

}  // namespace Escher
