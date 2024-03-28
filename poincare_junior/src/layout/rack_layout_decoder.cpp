#include "rack_layout_decoder.h"

namespace PoincareJ {

CodePoint CPLayoutDecoder::codePointAt(size_t index) const {
  if (index == m_end) {
    return UCodePointNull;
  }
  assert(0 <= index && index < m_end);

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
