#ifndef POINCARE_JUNIOR_LAYOUT_CODE_POINT_LAYOUT_H
#define POINCARE_JUNIOR_LAYOUT_CODE_POINT_LAYOUT_H

#include <ion/unicode/code_point.h>
#include <omgpj/bit.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class CodePointLayout {
 public:
  constexpr static uint8_t SubCodePointLayoutAtIndex(CodePoint value,
                                                     int index) {
    return Bit::getByteAtIndex(value, index);
  }
  static CodePoint GetCodePoint(const Tree* node);
  static CodePoint GetCombinedCodePoint(const Tree* node);
  static void GetName(const Tree* node, char* buffer, size_t bufferSize);
};

}  // namespace PoincareJ

#endif
