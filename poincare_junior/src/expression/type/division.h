#ifndef POINCARE_EXPRESSIONS_DIVISION_H
#define POINCARE_EXPRESSIONS_DIVISION_H

#include <poincare_junior/src/expression/expression.h>

namespace Poincare {

class Division final : public EExpression {
public:
  constexpr static size_t k_numberOfBlocksInNode = 1;
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex) {
    assert(blockIndex == 0);
    *block = DivisionBlock;
    return true;
  }

  static float Reduce(float a, float b) { return a / b; }
  static void BasicReduction(TypeBlock * block);
};

}

#endif
