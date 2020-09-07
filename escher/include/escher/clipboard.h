#ifndef ESCHER_CLIPBOARD_H
#define ESCHER_CLIPBOARD_H

#include <escher/text_field.h>
#include <poincare/layout.h>
#include <ion/unicode/utf8_helper.h>

class Clipboard {
public:
  static Clipboard * sharedClipboard();
  void store(const char * storedText, int length = -1);
  const char * storedText();
  void reset();
  void enterPython() { replaceCharForPython(true); }
  void exitPython() { replaceCharForPython(false); }
  static constexpr int k_bufferSize = TextField::maxBufferSize();
private:
  char m_textBuffer[k_bufferSize];
  void replaceCharForPython(bool entersPythonApp);
};


/* The order in which the text pairs are stored is important. Indeed when leaving
 * python, the text stored in the buffer is converted into an input for other
 * apps. Therefore if we want to convert "3**3" into "3^3", the function must
 * look for "**" paterns before "*". Otherwise, we will get "3××3". */
static constexpr int NumberOfPythonTextPairs = 12;
static constexpr UTF8Helper::TextPair PythonTextPairs[NumberOfPythonTextPairs] = {
  UTF8Helper::TextPair("√(\x11)", "sqrt(\x11)", true),
  UTF8Helper::TextPair("ℯ^(\x11)", "exp(\x11)", true),
  UTF8Helper::TextPair("log(\x11)", "log10(\x11)", true),
  UTF8Helper::TextPair("ln(\x11)", "log(\x11)", true),
  UTF8Helper::TextPair("ᴇ", "e"),
  UTF8Helper::TextPair("𝐢", "1j"),
  /* Since textPairs are also used to pair events, we need to keep both ^2 and ^
   * to get the desired behavior in python when using power or square key*/
  UTF8Helper::TextPair("^2", "**2"),
  UTF8Helper::TextPair("^", "**"),
  UTF8Helper::TextPair("π", "pi"),
  UTF8Helper::TextPair("×", "*"),
  UTF8Helper::TextPair("·", "*"),
  UTF8Helper::TextPair("][", "], ["),
};

#endif
