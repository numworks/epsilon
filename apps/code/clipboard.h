#ifndef PYTHON_CLIPBOARD_H
#define PYTHON_CLIPBOARD_H

#include <escher/clipboard.h>
#include <ion/unicode/utf8_helper.h>

namespace Code {

class Clipboard : public Escher::Clipboard {
 public:
  constexpr static int k_numberOfPythonTextPairs = 11;

  static Clipboard* sharedClipboard();
  static const UTF8Helper::TextPair* PythonTextPairs();
  static bool ShouldReplaceLetterE(const char* text, size_t length,
                                   size_t position);
  void enterPython() { replaceCharForPython(true); }
  void exitPython() { replaceCharForPython(false); }

 private:
  static int s_replacementRuleStartingPoint;
  void replaceCharForPython(bool entersPythonApp);
};

}  // namespace Code

#endif
