#include <escher/clipboard.h>
#include <algorithm>

static Clipboard s_clipboard;

Clipboard * Clipboard::sharedClipboard() {
  return &s_clipboard;
}

void Clipboard::store(const char * storedText, int length) {
  strlcpy(m_textBuffer, storedText, length == -1 ? TextField::maxBufferSize() : std::min(TextField::maxBufferSize(), length + 1));
}

void Clipboard::reset() {
  strlcpy(m_textBuffer, "", 1);
}
