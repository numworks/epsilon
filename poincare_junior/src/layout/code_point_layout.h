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

  // Print name in buffer and return end
  static char* CopyName(const Tree* node, char* buffer, size_t bufferSize);

  static bool IsCodePoint(const Tree* node, CodePoint cp);
};

}  // namespace PoincareJ

#endif
