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

Tree* CodePointLayout::Push(CodePoint codePoint) {
  if (codePoint < 128) {
    return SharedTreeStack->pushAsciiCodePointLayout(codePoint);
  }
  return SharedTreeStack->pushUnicodeCodePointLayout(codePoint);
}
Tree* CodePointLayout::PushCombined(CodePoint codePoint,
                                    CodePoint combinedCodepoint) {
  return SharedTreeStack->pushCombinedCodePointsLayout(codePoint,
                                                       combinedCodepoint);
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

bool CodePointLayout::IsCodePoint(const Tree* l, CodePoint codePoint) {
  return l->isCodePointLayout() && GetCodePoint(l) == codePoint;
}
bool CodePointLayout::IsCombinedCodePoint(const Tree* l, CodePoint codePoint,
                                          CodePoint combinedCodePoint) {
  return l->isCombinedCodePointsLayout() && GetCodePoint(l) == codePoint &&
         GetCombinedCodePoint(l) == combinedCodePoint;
}

}  // namespace Poincare::Internal
