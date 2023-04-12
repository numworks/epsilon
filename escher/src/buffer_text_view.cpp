#include <assert.h>
#include <escher/buffer_text_view.h>
#include <escher/i18n.h>
#include <poincare/print.h>
#include <string.h>

namespace Escher {

void AbstractBufferTextView::setText(const char* text) {
  strlcpy(buffer(), text, maxTextSize());
  markRectAsDirty(bounds());
}

void AbstractBufferTextView::setMessageWithPlaceholders(I18n::Message message,
                                                        ...) {
  va_list args;
  va_start(args, message);
  privateSetMessageWithPlaceholders(message, args);
  va_end(args);
}

void AbstractBufferTextView::privateSetMessageWithPlaceholders(
    I18n::Message message, va_list args) {
  int length = Poincare::Print::PrivateCustomPrintf(
      buffer(), maxTextSize(), I18n::translate(message), args);
  assert(length < maxTextSize());
  (void)length;
  markRectAsDirty(bounds());
}

void AbstractBufferTextView::appendText(const char* text) {
  size_t previousTextLength = strlen(buffer());
  if (previousTextLength < maxTextSize() - 1) {
    strlcpy(buffer() + previousTextLength, text,
            maxTextSize() - previousTextLength);
  }
}

KDSize AbstractBufferTextView::minimalSizeForOptimalDisplay() const {
  return KDFont::Font(font())->stringSize(text());
}

}  // namespace Escher
