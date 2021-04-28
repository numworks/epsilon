#include "clipboard.h"

using namespace Escher;

namespace Code {

Clipboard * Clipboard::sharedClipboard() {
  assert(sizeof(Clipboard) == sizeof(Escher::Clipboard));
  return static_cast<Clipboard *>(Escher::Clipboard::sharedClipboard());
}

const UTF8Helper::TextPair * Clipboard::PythonTextPairs() {
  /* The order in which the text pairs are stored is important. Indeed when
   * leaving python, the text stored in the buffer is converted into an input
   * for other apps. Therefore if we want to convert "3**3" into "3^3", the
   * function must look for "**" paterns before "*". Otherwise, we will get
   * "3××3". */
  static constexpr UTF8Helper::TextPair k_pythonTextPairs[k_numberOfPythonTextPairs] = {
    UTF8Helper::TextPair("√(\x11)", "sqrt(\x11)", true),
    UTF8Helper::TextPair("ℯ^(\x11)", "exp(\x11)", true),
    UTF8Helper::TextPair("log(\x11)", "log10(\x11)", true),
    UTF8Helper::TextPair("ln(\x11)", "log(\x11)", true),
    UTF8Helper::TextPair("ᴇ", "e"),
    UTF8Helper::TextPair("𝐢", "1j"),
    /* Since TextPairs are also used to pair events, we need to keep both ^2
     * and ^ to get the desired behavior in python when using power or square
     * key. */
    UTF8Helper::TextPair("^2", "**2"),
    UTF8Helper::TextPair("^", "**"),
    UTF8Helper::TextPair("π", "pi"),
    UTF8Helper::TextPair("×", "*"),
    UTF8Helper::TextPair("·", "*"),
    UTF8Helper::TextPair("][", "], ["),
  };
  return k_pythonTextPairs;
}

void Clipboard::replaceCharForPython(bool entersPythonApp) {
  UTF8Helper::TryAndReplacePatternsInStringByPatterns(m_textBuffer, TextField::maxBufferSize(), PythonTextPairs(), k_numberOfPythonTextPairs, entersPythonApp);
}

}
