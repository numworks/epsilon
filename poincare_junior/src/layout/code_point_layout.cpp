#include "code_point_layout.h"

#include <ion/unicode/utf8_decoder.h>

namespace PoincareJ {

CodePoint CodePointLayout::GetCodePoint(const Tree* node) {
  uint32_t value;
  memcpy(&value, node->block()->next(), sizeof(uint32_t));
  return CodePoint(value);
}

void CodePointLayout::GetName(const Tree* node, char* buffer,
                              size_t bufferSize) {
  CodePoint c = GetCodePoint(node);
  size_t size = UTF8Decoder::CodePointToChars(c, buffer, bufferSize);
  buffer[size] = 0;
}

}  // namespace PoincareJ
