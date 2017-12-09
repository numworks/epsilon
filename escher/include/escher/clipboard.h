#ifndef ESCHER_CLIPBOARD_H
#define ESCHER_CLIPBOARD_H

#include <escher/text_field.h>

class Clipboard final {
public:
  static Clipboard * sharedClipboard() { return &s_clipboard; }
  void store(const char * storedText);
  const char * storedText() { return m_textBuffer; }
  void reset() { m_textBuffer[0] = 0; }
private:
  static Clipboard s_clipboard;
  char m_textBuffer[TextField::maxBufferSize()];
};

#endif
