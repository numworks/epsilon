#include "rack_layout_decoder.h"

#include <omg/unicode_helper.h>

namespace Poincare::Internal {

const CPL* CPL::CodePointLSearch(const CPL* s, CodePoint c, const CPL* stop) {
  while (s != stop && *s != 0) {
    if (*s == c) {
      return s;
    }
    s++;
  }
  return s;
}

int CPL::compareWithNullTerminatedString(int length, const char* string) const {
  Poincare::Internal::CPLayoutDecoder decoder(
      reinterpret_cast<const Poincare::Internal::Tree*>(this), 0, length);
  return OMG::CompareDecoderWithNullTerminatedString(&decoder, string);
}

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

}  // namespace Poincare::Internal
