#ifndef POINCARE_LAYOUT_CODE_POINT_LAYOUT_H
#define POINCARE_LAYOUT_CODE_POINT_LAYOUT_H

#include <omgpj/bit.h>
#include <ion/unicode/code_point.h>

namespace PoincareJ {

class CodePointLayout {
public:
  constexpr static uint8_t SubCodePointLayoutAtIndex(CodePoint value, int index) {
    return Bit::getByteAtIndex(value, index);
  }
};

}

#endif
