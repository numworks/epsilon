#include <escher/clipboard.h>
#include <escher/text_field.h>
#include <ion/clipboard.h>
#include <ion/unicode/utf8_decoder.h>
#include <poincare/expression.h>
#include <poincare_junior/src/memory/tree.h>

#include <algorithm>

namespace Escher {

static Clipboard s_clipboard;

Clipboard* Clipboard::SharedClipboard() { return &s_clipboard; }

void Clipboard::store(const char* storedText, int length) {
  int maxSize = TextField::MaxBufferSize();
  if (length == -1 || length + 1 > maxSize) {
    // Make sure the text isn't truncated in the middle of a code point.
    length = std::min(maxSize - 1, static_cast<int>(strlen(storedText)));
    /* length can't be greater than strlen(storedText) to prevent any out of
     * array bound access. */
    assert(  // to trigger fuzzer
        length == 0 ||
        UTF8Decoder::IsTheEndOfACodePoint(&storedText[length - 1], storedText));
    while (length > 0 && !UTF8Decoder::IsTheEndOfACodePoint(
                             &storedText[length - 1], storedText)) {
      length--;
    }
  }
  assert(length >= 0);
  assert(length == 0 || UTF8Decoder::IsTheEndOfACodePoint(
                            &storedText[length - 1], storedText));
  strlcpy(m_textBuffer, storedText, length + 1);
  Ion::Clipboard::write(m_textBuffer);
}

void Clipboard::storeLayout(Poincare::Layout layout) {
  int size = layout.tree()->treeSize();
  if (size < k_bufferSize) {
    memcpy(m_treeBuffer, layout.tree(), size);
  }
  // Serialize in case we need it in python or outside epsilon
  layout.serializeForParsing(m_textBuffer, k_bufferSize);
  // TODO PCJ check that it fits
  Ion::Clipboard::write(m_textBuffer);
}

const char* Clipboard::storedText() {
  const char* systemText = Ion::Clipboard::read();
  if (systemText) {
    return systemText;
  }

  /* In order to allow copy/paste of empty formulas, we need to add empty
   * layouts between empty system parenthesis. This way, when the expression
   * is parsed, it is recognized as a proper formula and appears with the
   * correct visual layout. Without this process, copying an empty integral then
   * pasting it gives : int((), x, (), ()) instead of drawing an empty integral.
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

  UTF8Helper::TryAndReplacePatternsInStringByPatterns(
      m_textBuffer, TextField::MaxBufferSize(), textPairs, numberOfPairs, true);
  return m_textBuffer;
}

Poincare::Layout Clipboard::storedLayout() {
  const char* systemText = Ion::Clipboard::read();
  if (systemText) {
    return Poincare::Expression::Parse(systemText, nullptr)
        .createLayout(Poincare::Preferences::PrintFloatMode::Decimal, 14,
                      nullptr);
  }
  return Poincare::Layout::Builder(
      reinterpret_cast<const PoincareJ::Tree*>(m_treeBuffer));
}

void Clipboard::reset() {
  strlcpy(m_textBuffer, "", 1);
  /* As we do not want to empty the user's computer's clipboard when entering
   * exam mode, we do not reset Ion::Clipboard. */
}

}  // namespace Escher
