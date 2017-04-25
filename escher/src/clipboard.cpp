#include <escher/clipboard.h>

static Clipboard s_clipboard;

Clipboard * Clipboard::sharedClipboard() {
  return &s_clipboard;
}

void Clipboard::store(const char * storedText) {
  strlcpy(m_textBuffer, storedText, TextField::maxBufferSize());
}

const char * Clipboard::storedText() {
  return m_textBuffer;
}

void Clipboard::reset() {
  strlcpy(m_textBuffer, "", 1);
}
