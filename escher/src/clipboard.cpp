#include <escher/clipboard.h>
#include <escher/text_field.h>
#include <ion/clipboard.h>
#include <algorithm>
#include <ion/unicode/utf8_decoder.h>

namespace Escher {

static Clipboard s_clipboard(true);

Clipboard * Clipboard::sharedClipboard() {
  return &s_clipboard;
}

static Clipboard s_storeBuffer(false);

Clipboard * Clipboard::sharedStoreBuffer() {
  return &s_storeBuffer;
}

void Clipboard::store(const char * storedText, int length) {
  int maxSize = TextField::maxBufferSize();
  if (length == -1 || length + 1 > maxSize) {
    // Make sure the text isn't truncated in the middle of a code point.
    length = std::min(maxSize - 1, static_cast<int>(strlen(storedText)));
    /* length can't be greater than strlen(storedText) to prevent any out of
     * array bound access. */
    while (UTF8Decoder::IsInTheMiddleOfACodePoint(storedText[length])) {
      length--;
    }
  }
  assert(length >= 0 && !UTF8Decoder::IsInTheMiddleOfACodePoint(storedText[length]));
  strlcpy(m_textBuffer, storedText, length + 1);
  if (m_isSystem) {
    Ion::Clipboard::write(m_textBuffer);
  }
}

const char * Clipboard::storedText() {
  if (m_isSystem) {
    const char * systemText = Ion::Clipboard::read();
    if (systemText) {
      return systemText;
    }
  }

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

  /* Size is maxBufferSize - 1 to ensure null termination. */
  UTF8Helper::TryAndReplacePatternsInStringByPatterns(m_textBuffer, TextField::maxBufferSize() - 1, textPairs, numberOfPairs, true);
  return m_textBuffer;
}

void Clipboard::reset() {
  strlcpy(m_textBuffer, "", 1);
  /* As we do not want to empty the user's computer's clipboard when entering
   * exam mode, we do not reset Ion::Clipboard. */
}

}
