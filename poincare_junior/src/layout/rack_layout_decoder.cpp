#include "rack_layout_decoder.h"

namespace PoincareJ {

CodePoint CPLayoutDecoder::codePointAt(size_t index) const {
  if (index == m_end) {
    return UCodePointNull;
  }
  assert(0 <= index && index < m_end);
  /* This was previously m_firstCodePoint + index * k_codePointLayoutSize; but
   * was changed since codepoints are of varying sizes. It is less efficient but
   * maybe having the CPLayoutDecoder limited to ascii codepoints is
   * sufficient. We will probably never need to parse non-ascii symbols like π
   * as part of a word. Decoders needs to be refactored anyway. */
  const Tree* codePoint = m_firstCodePoint;
  for (int i = 0; i < index; i++) {
    codePoint = codePoint->nextTree();
  }
  if (!codePoint->isCodePointLayout()) {
    return UCodePointNull;
  }
  return CodePointLayout::GetCodePoint(codePoint);
}

}  // namespace PoincareJ
