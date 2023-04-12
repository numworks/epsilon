#ifndef POINCARE_EXPRESSION_FLOAT_H
#define POINCARE_EXPRESSION_FLOAT_H

#include <omgpj/bit.h>

#include <bit>

namespace PoincareJ {

class Float {
 public:
  constexpr static uint8_t SubFloatAtIndex(float value, int index) {
    return Bit::getByteAtIndex(std::bit_cast<uint32_t>(value), index);
  }
};

}  // namespace PoincareJ

#endif
