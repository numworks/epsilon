#include <assert.h>
#include <escher/buffer_text_view.h>
#include <escher/i18n.h>
#include <poincare/old/print.h>
#include <string.h>

namespace Escher {

void AbstractBufferTextView::setText(const char* text) {
  strlcpy(buffer(), text, maxTextSize());
  markWholeFrameAsDirty();
}

void AbstractBufferTextView::setMessageWithPlaceholders(I18n::Message message,
                                                        ...) {
  va_list args;
  va_start(args, message);
  bool result = privateSetMessageWithPlaceholders(message, args);
  assert(result);
  (void)result;
  va_end(args);
}

bool AbstractBufferTextView::unsafeSetMessageWithPlaceholders(
    I18n::Message message, ...) {
  va_list args;
  va_start(args, message);
  bool result = privateSetMessageWithPlaceholders(message, args);
  va_end(args);
  return result;
}

bool AbstractBufferTextView::privateSetMessageWithPlaceholders(
    I18n::Message message, va_list args) {
  int length = Poincare::Print::PrivateCustomPrintf(
      buffer(), maxTextSize(), I18n::translate(message), args);
  markWholeFrameAsDirty();
  return length < maxTextSize();
}

void AbstractBufferTextView::appendText(const char* text) {
  int previousTextLength = strlen(buffer());
  if (previousTextLength < maxTextSize() - 1) {
    strlcpy(buffer() + previousTextLength, text,
            maxTextSize() - previousTextLength);
  }
}

KDSize AbstractBufferTextView::minimalSizeForOptimalDisplay() const {
  return KDFont::Font(font())->stringSize(text());
}

}  // namespace Escher
