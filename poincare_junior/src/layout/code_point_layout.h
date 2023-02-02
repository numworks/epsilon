#ifndef POINCARE_LAYOUT_CODE_POINT_LAYOUT_H
#define POINCARE_LAYOUT_CODE_POINT_LAYOUT_H

#include <omgpj/bit.h>
#include <ion/unicode/code_point.h>
#include <ion/unicode/utf8_decoder.h>
#include <poincare_junior/src/memory/node.h>

namespace PoincareJ {

class CodePointLayout {
public:
  constexpr static uint8_t SubCodePointLayoutAtIndex(CodePoint value, int index) {
    return Bit::getByteAtIndex(value, index);
  }
  static void GetName(const Node node, char * buffer, size_t bufferSize) {
    CodePoint c = CodePoint(*reinterpret_cast<uint32_t *>(node.block()->nextNth(1)));
    UTF8Decoder::CodePointToChars(c, buffer, bufferSize);
  }
};

}

#endif
