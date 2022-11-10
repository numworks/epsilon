#ifndef POINCARE_EXPRESSIONS_POWER_H
#define POINCARE_EXPRESSIONS_POWER_H

#include <cmath>
#include <poincare_junior/src/expression/expression.h>

namespace Poincare {

class Power final : public EExpression {
public:
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex) {
    assert(blockIndex == 0);
    *block = PowerBlock;
    return true;
  }
  static constexpr size_t k_numberOfBlocksInNode = 1;
  static float Reduce(float a, float b) { return std::pow(a, b); }
};

}

#endif
