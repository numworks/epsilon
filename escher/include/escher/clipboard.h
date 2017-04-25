#ifndef ESCHER_CLIPBOARD_H
#define ESCHER_CLIPBOARD_H

#include <escher/text_field.h>

class Clipboard {
public:
  static Clipboard * sharedClipboard();
  void store(const char * storedText);
  const char * storedText();
  void reset();
private:
  char m_textBuffer[TextField::maxBufferSize()];
};

#endif
