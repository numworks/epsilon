#ifndef PYTHON_CLIPBOARD_H
#define PYTHON_CLIPBOARD_H

#include <escher/clipboard.h>
#include <ion/unicode/utf8_helper.h>

namespace Code {

class Clipboard : public Escher::Clipboard {
public:
  static constexpr int k_numberOfPythonTextPairs = 12;

  static Clipboard * sharedClipboard();
  static const UTF8Helper::TextPair * PythonTextPairs();
  void enterPython() { replaceCharForPython(true); }
  void exitPython() { replaceCharForPython(false); }
private:
  void replaceCharForPython(bool entersPythonApp);
};

}

#endif
