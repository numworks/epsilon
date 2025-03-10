#include <escher/clipboard.h>
#include <escher/text_field.h>
#include <ion/clipboard.h>
#include <omg/utf8_decoder.h>
#include <poincare/expression.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/memory/tree.h>

#include <algorithm>

namespace Escher {

Clipboard* Clipboard::SharedClipboard() {
  static Clipboard s_clipboard;
  return &s_clipboard;
}

void Clipboard::storeText(const char* text, int length) {
  int maxSize = TextField::MaxBufferSize();
  if (length == -1 || length + 1 > maxSize) {
    // Make sure the text isn't truncated in the middle of a code point.
    length = std::min(maxSize - 1, static_cast<int>(strlen(text)));
    /* length can't be greater than strlen(storedText) to prevent any out of
     * array bound access. */
    assert(  // to trigger fuzzer
        length == 0 ||
        UTF8Decoder::IsTheEndOfACodePoint(&text[length - 1], text));
    while (length > 0 &&
           !UTF8Decoder::IsTheEndOfACodePoint(&text[length - 1], text)) {
      length--;
    }
  }
  assert(length >= 0);
  assert(length == 0 ||
         UTF8Decoder::IsTheEndOfACodePoint(&text[length - 1], text));
  strlcpy(m_textBuffer, text, length + 1);
  Ion::Clipboard::write(m_textBuffer);
  m_bufferState = TreeOutdated;
}

void Clipboard::storeLayout(Poincare::Layout layout) {
  int size = layout.tree()->treeSize();
  if (size < k_bufferSize) {
    memcpy(m_treeBuffer, layout.tree(), size);
  }
  // Serialize in case we need it in a text field/area or outside epsilon
  // updateTextFromTree();
  // TODO_PCJ check that it fits
  Ion::Clipboard::write(m_textBuffer);
  m_bufferState = TextOutdated;
}

const char* Clipboard::storedText() {
  const char* systemText = Ion::Clipboard::read();
  if (systemText) {
    return systemText;
  }

  if (m_bufferState == TextOutdated) {
    updateTextFromTree();
  }
#if 0
  // TODO remove if nothing is broken
  /* In order to allow copy/paste of empty formulas, we need to add empty
   * layouts between empty system parenthesis. This way, when the expression
   * is parsed, it is recognized as a proper formula and appears with the
   * correct visual layout. Without this process, copying an empty integral
   * then pasting it gives : int((), x, (), ()) instead of drawing an empty
   * integral.
   *
   * Furthermore, in case the user switches from linear to natural writing
   * mode we need to add an empty layout between parenthesis to allow proper
   * layout construction. */
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
#endif
  return m_textBuffer;
}

Poincare::Layout Clipboard::storedLayout() {
  const char* systemText = Ion::Clipboard::read();
  if (systemText) {
    // TODO: Is it needed to use an expression here ? why not layout directly ?
    return Poincare::Expression::Parse(systemText, nullptr)
        .createLayout(Poincare::Preferences::PrintFloatMode::Decimal,
                      Poincare::PrintFloat::k_maxNumberOfSignificantDigits,
                      nullptr);
  }
  if (m_bufferState == TreeOutdated) {
    updateTreeFromText();
  }
  return privateStoredLayout();
}

void Clipboard::reset() {
  strlcpy(m_textBuffer, "", 1);
  const Poincare::Internal::Tree* emptyRack = Poincare::Internal::KRackL();
  memcpy(m_treeBuffer, emptyRack, emptyRack->treeSize());
  m_bufferState = Updated;
  /* As we do not want to empty the user's computer's clipboard when entering
   * exam mode, we do not reset Ion::Clipboard. */
}

void Clipboard::updateTreeFromText() {
  Poincare::Layout layout = Poincare::Layout::Parse(m_textBuffer);
  int size = layout.tree()->treeSize();
  if (size < k_bufferSize) {
    memcpy(m_treeBuffer, layout.tree(), size);
  }
  m_bufferState = Updated;
}

void Clipboard::updateTextFromTree() {
  privateStoredLayout().serialize(m_textBuffer, k_bufferSize);
  m_bufferState = Updated;
}

Poincare::Layout Clipboard::privateStoredLayout() const {
  // TODO: Assert that SharedTreeStack is initialized
  return Poincare::Layout::Builder(
      reinterpret_cast<const Poincare::Internal::Tree*>(m_treeBuffer));
}

}  // namespace Escher
