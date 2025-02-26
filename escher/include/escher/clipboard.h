#ifndef ESCHER_CLIPBOARD_H
#define ESCHER_CLIPBOARD_H

#include <escher/text_field.h>
#include <poincare/layout.h>

namespace Escher {

class Clipboard {
 public:
  static Clipboard* SharedClipboard();
  void storeText(const char* text, int length = -1);
  void storeLayout(Poincare::Layout layout);
  const char* storedText();
  Poincare::Layout storedLayout();
  void reset();
  constexpr static int k_bufferSize = TextField::MaxBufferSize();

 protected:
  Clipboard() { reset(); }
  void updateTextFromTree();
  Poincare::Layout privateStoredLayout() const;

  /* TODO store either text or layout in an union and convert as needed ? */
  char m_textBuffer[k_bufferSize];
  char m_treeBuffer[k_bufferSize];
};

}  // namespace Escher

#endif
