#include "code_point_layout.h"

#include <omg/utf8_decoder.h>
#include <poincare/src/memory/tree_stack.h>

namespace Poincare::Internal {

CodePoint CodePointLayout::GetCodePoint(const Tree* l) {
  if (l->isAsciiCodePointLayout()) {
    return CodePoint(l->nodeValueBlock(0)->get<uint8_t>());
  }
  assert(l->isUnicodeCodePointLayout() || l->isCombinedCodePointsLayout());
  return CodePoint(l->nodeValueBlock(0)->get<uint32_t>());
}

CodePoint CodePointLayout::GetCombinedCodePoint(const Tree* l) {
  return CodePoint(l->nodeValueBlock(4)->get<uint32_t>());
}

Tree* CodePointLayout::Push(CodePoint cp) {
  if (cp < 128) {
    return SharedTreeStack->pushAsciiCodePointLayout(cp);
  }
  return SharedTreeStack->pushUnicodeCodePointLayout(cp);
}

char* CodePointLayout::CopyName(const Tree* l, char* buffer,
                                size_t bufferSize) {
  CodePoint c = GetCodePoint(l);
  size_t size = UTF8Decoder::CodePointToChars(c, buffer, bufferSize);
  if (l->isCombinedCodePointsLayout()) {
    CodePoint c = GetCombinedCodePoint(l);
    size += UTF8Decoder::CodePointToChars(c, buffer + size, bufferSize - size);
  }
  buffer[size] = 0;
  return &buffer[size];
}

bool CodePointLayout::IsCodePoint(const Tree* l, CodePoint cp) {
  return l->isCodePointLayout() && GetCodePoint(l) == cp;
}

}  // namespace Poincare::Internal
