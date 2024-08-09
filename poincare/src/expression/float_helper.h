#ifndef POINCARE_EXPRESSION_FLOAT_HELPER_H
#define POINCARE_EXPRESSION_FLOAT_HELPER_H

#include <omg/bit_helper.h>
#include <poincare/src/expression/sign.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>

#include <bit>

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
  static float FloatTo(const Tree* e) {
    return e->nodeValueBlock(0)->get<float>();
  }
  static double DoubleTo(const Tree* e) {
    return e->nodeValueBlock(0)->get<double>();
  }
  static double To(const Tree* e) {
    assert(e->isFloat());
    return e->isSingleFloat() ? FloatTo(e) : DoubleTo(e);
  }
  static bool SetSign(Tree* e, NonStrictSign sign);
};

}  // namespace Poincare::Internal

#endif
