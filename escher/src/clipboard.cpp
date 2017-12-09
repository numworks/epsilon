#include <escher/clipboard.h>

Clipboard Clipboard::s_clipboard;

void Clipboard::store(const char * storedText) {
  strlcpy(m_textBuffer, storedText, TextField::maxBufferSize());
}
