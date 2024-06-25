#ifndef POINCARE_EXPRESSION_FLOAT_HELPER_H
#define POINCARE_EXPRESSION_FLOAT_HELPER_H

#include <omg/bit_helper.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>

#include <bit>

#include "sign.h"

namespace Poincare::Internal {

class FloatHelper {
 public:
  constexpr static uint8_t SubFloatAtIndex(float value, int index) {
    return OMG::BitHelper::getByteAtIndex(std::bit_cast<uint32_t>(value),
                                          index);
  }
  constexpr static uint8_t SubFloatAtIndex(double value, int index) {
    return OMG::BitHelper::getByteAtIndex(std::bit_cast<uint64_t>(value),
                                          index);
  }
  static float FloatTo(const Tree* tree) {
    return tree->nodeValueBlock(0)->get<float>();
  }
  static double DoubleTo(const Tree* tree) {
    return tree->nodeValueBlock(0)->get<double>();
  }
  static double To(const Tree* tree) {
    assert(tree->isFloat());
    return tree->isSingleFloat() ? FloatTo(tree) : DoubleTo(tree);
  }
  static bool SetSign(Tree* tree, NonStrictSign sign);
};

}  // namespace Poincare::Internal

#endif
