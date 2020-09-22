#include <escher/clipboard.h>
#include <escher/text_field.h>
#include <algorithm>

static Clipboard s_clipboard;

Clipboard * Clipboard::sharedClipboard() {
  return &s_clipboard;
}

void Clipboard::store(const char * storedText, int length) {
  strlcpy(m_textBuffer, storedText, length == -1 ? TextField::maxBufferSize() : std::min(TextField::maxBufferSize(), length + 1));
}

const char * Clipboard::storedText() {
  /* In order to allow copy/paste of empty formulas, we need to add empty
   * layouts between empty system parenthesis. This way, when the expression
   * is parsed, it is recognized as a proper formula and appears with the correct
   * visual layout.
   * Without this process, copying an empty integral then pasting it gives :
   * int((), x, (), ()) instead of drawing an empty integral.
   *
   * Furthermore, in case the user switches from linear to natural writing mode
   * we need to add an empty layout between parenthesis to allow proper layout
   * construction. */
  constexpr int numberOfPairs = 6;
  constexpr UTF8Helper::TextPair textPairs[numberOfPairs] = {
    UTF8Helper::TextPair("()", "(\x11)"),
    UTF8Helper::TextPair("[]", "[\x11]"),
    UTF8Helper::TextPair("[,", "[\x11,"),
    UTF8Helper::TextPair(",,", ",\x11,"),
    UTF8Helper::TextPair(",]", ",\x11]"),
    UTF8Helper::TextPair("\x12\x13", "\x12\x11\x13"),
  };

  UTF8Helper::TryAndReplacePatternsInStringByPatterns(m_textBuffer, TextField::maxBufferSize(), (UTF8Helper::TextPair *) &textPairs, numberOfPairs, true);
  return m_textBuffer;
}

void Clipboard::reset() {
  strlcpy(m_textBuffer, "", 1);
}

void Clipboard::replaceCharForPython(bool entersPythonApp) {
  UTF8Helper::TryAndReplacePatternsInStringByPatterns((char *)m_textBuffer, TextField::maxBufferSize(), (UTF8Helper::TextPair *)&PythonTextPairs, NumberOfPythonTextPairs, entersPythonApp);
}
