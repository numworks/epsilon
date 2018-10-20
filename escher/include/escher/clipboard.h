#ifndef ESCHER_CLIPBOARD_H
#define ESCHER_CLIPBOARD_H

#include <escher/text_field.h>
#include <poincare/layout.h>

class Clipboard {
public:
  static Clipboard * sharedClipboard();
  void store(const char * storedText);
  void store(Poincare::Layout layoutR);
  const char * storedText();
  void reset();
private:
  char m_textBuffer[TextField::maxBufferSize()];
};

#endif
