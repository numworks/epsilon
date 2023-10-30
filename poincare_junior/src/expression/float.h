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
  constexpr static uint8_t SubFloatAtIndex(double value, int index) {
    return Bit::getByteAtIndex(std::bit_cast<uint64_t>(value), index);
  }
  static float FloatTo(const Tree *tree) {
    /* uint32_t can be fetched unaligned but not floats so we need this
     * intermediary step. volatile is used to prevent the compiler from
     * optimizing the uint32_t away but is too much : moving the value in an
     * integer register is sufficient we don't need to force it into memory. */
    volatile uint32_t value =
        *reinterpret_cast<const uint32_t *>(tree->block()->next());
    return std::bit_cast<float>(value);
  }
  static double DoubleTo(const Tree *tree) {
    volatile uint64_t value =
        *reinterpret_cast<const uint64_t *>(tree->block()->next());
    return std::bit_cast<double>(value);
  }
  static double To(const Tree *tree) {
    assert(tree->type().isFloat());
    return tree->type() == BlockType::SingleFloat ? FloatTo(tree)
                                                  : DoubleTo(tree);
  }
};

// Helper from T  = float|double to corresponding BlockType
template <class T>
struct FloatType;

template <>
struct FloatType<float> {
  static constexpr BlockType type = BlockType::SingleFloat;
};

template <>
struct FloatType<double> {
  static constexpr BlockType type = BlockType::DoubleFloat;
};

}  // namespace PoincareJ

#endif
