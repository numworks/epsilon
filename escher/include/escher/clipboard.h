#ifndef ESCHER_CLIPBOARD_H
#define ESCHER_CLIPBOARD_H

#include <escher/text_field.h>
#include <poincare/layout.h>

namespace Escher {

class Clipboard {
 public:
  static Clipboard* SharedClipboard();
  void store(const char* storedText, int length = -1);
  const char* storedText();
  void reset();
  constexpr static int k_bufferSize = TextField::MaxBufferSize();

 protected:
  char m_textBuffer[k_bufferSize];
};

}  // namespace Escher

#endif
