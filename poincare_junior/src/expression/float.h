#ifndef POINCARE_EXPRESSION_FLOAT_H
#define POINCARE_EXPRESSION_FLOAT_H

#include <omgpj/bit.h>
#include <poincare_junior/src/memory/tree.h>

#include <bit>

namespace PoincareJ {

class FloatNode {
 public:
  constexpr static uint8_t SubFloatAtIndex(float value, int index) {
    return Bit::getByteAtIndex(std::bit_cast<uint32_t>(value), index);
  }
  constexpr static uint8_t SubFloatAtIndex(double value, int index) {
    return Bit::getByteAtIndex(std::bit_cast<uint64_t>(value), index);
  }
  static float FloatTo(const Tree *tree) {
    return tree->nodeValueBlock(0)->get<float>();
  }
  static double DoubleTo(const Tree *tree) {
    return tree->nodeValueBlock(0)->get<double>();
  }
  static double To(const Tree *tree) {
    assert(tree->isFloat());
    return tree->isSingleFloat() ? FloatTo(tree) : DoubleTo(tree);
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
