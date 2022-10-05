#ifndef ESCHER_CLIPBOARD_H
#define ESCHER_CLIPBOARD_H

#include <escher/text_field.h>
#include <poincare/layout.h>

namespace Escher {

class Clipboard {
public:
  static Clipboard * sharedClipboard();
  void store(const char * storedText, int length = -1);
  const char * storedText();
  void reset();
  constexpr static int k_bufferSize = TextField::maxBufferSize();
protected:
  char m_textBuffer[k_bufferSize];
};

}

#endif
