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

void Clipboard::reset() {
  strlcpy(m_textBuffer, "", 1);
}

void Clipboard::replaceCharForPython(bool entersPythonApp) {
  UTF8Helper::tryAndReplacePatternsInStringByPatterns((char *)m_textBuffer, TextField::maxBufferSize(), (UTF8Helper::TextPair *)&PythonTextPairs, NumberOfPythonTextPairs, entersPythonApp);
}