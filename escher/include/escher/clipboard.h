#ifndef ESCHER_CLIPBOARD_H
#define ESCHER_CLIPBOARD_H

#include <escher/text_field.h>
#include <poincare/layout.h>

namespace Escher {

class Clipboard {
public:
  Clipboard(bool isSystemClipboard) : m_isSystem(isSystemClipboard) {}
  static Clipboard * sharedClipboard();
  static Clipboard * sharedStoreBuffer();
  void store(const char * storedText, int length = -1);
  const char * storedText();
  void reset();
  constexpr static int k_bufferSize = TextField::maxBufferSize();
protected:
  bool m_isSystem;
  char m_textBuffer[k_bufferSize];
};

}

#endif
