#include <escher/clipboard.h>

static Clipboard s_clipboard;

static inline int minInt(int x, int y) { return x < y ? x : y; }

Clipboard * Clipboard::sharedClipboard() {
  return &s_clipboard;
}

void Clipboard::store(const char * storedText, int length) {
  strlcpy(m_textBuffer, storedText, length == -1 ? TextField::maxBufferSize() : minInt(TextField::maxBufferSize(), length + 1));
}

void Clipboard::reset() {
  strlcpy(m_textBuffer, "", 1);
}
