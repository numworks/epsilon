#include "code_point_layout.h"

#include <ion/unicode/utf8_decoder.h>

namespace PoincareJ {

CodePoint CodePointLayout::GetCodePoint(const Tree* node) {
  return CodePoint(node->nodeValueBlock(0)->get<uint32_t>());
}

CodePoint CodePointLayout::GetCombinedCodePoint(const Tree* node) {
  return CodePoint(node->nodeValueBlock(4)->get<uint32_t>());
}

char* CodePointLayout::CopyName(const Tree* node, char* buffer,
                                size_t bufferSize) {
  CodePoint c = GetCodePoint(node);
  size_t size = UTF8Decoder::CodePointToChars(c, buffer, bufferSize);
  if (node->isCombinedCodePointsLayout()) {
    CodePoint c = GetCombinedCodePoint(node);
    size += UTF8Decoder::CodePointToChars(c, buffer + size, bufferSize - size);
  }
  buffer[size] = 0;
  return &buffer[size];
}

bool CodePointLayout::IsCodePoint(const Tree* node, CodePoint cp) {
  return node->isCodePointLayout() && GetCodePoint(node) == cp;
}

}  // namespace PoincareJ
