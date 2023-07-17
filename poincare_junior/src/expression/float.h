#ifndef POINCARE_EXPRESSION_FLOAT_H
#define POINCARE_EXPRESSION_FLOAT_H

#include <omgpj/bit.h>
#include <poincare_junior/src/memory/tree.h>

#include <bit>

namespace PoincareJ {

class Float {
 public:
  constexpr static uint8_t SubFloatAtIndex(float value, int index) {
    return Bit::getByteAtIndex(std::bit_cast<uint32_t>(value), index);
  }
  static float To(const Tree *tree) {
    volatile const uint32_t value =
        *reinterpret_cast<const uint32_t *>(tree->block()->next());
    return std::bit_cast<float>(value);
  }
};

}  // namespace PoincareJ

#endif
