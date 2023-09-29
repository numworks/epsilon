#include "code_point_layout.h"

#include <ion/unicode/utf8_decoder.h>

namespace PoincareJ {

CodePoint CodePointLayout::GetCodePoint(const Tree* node) {
  return CodePoint(
      *reinterpret_cast<const uint32_t*>(node->block()->nextNth(1)));
}

void CodePointLayout::GetName(const Tree* node, char* buffer,
                              size_t bufferSize) {
  CodePoint c = GetCodePoint(node);
  size_t size = UTF8Decoder::CodePointToChars(c, buffer, bufferSize);
  buffer[size] = 0;
}

}  // namespace PoincareJ
