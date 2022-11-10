#ifndef POINCARE_EXPRESSIONS_ADDITION_H
#define POINCARE_EXPRESSIONS_ADDITION_H

#include "n_ary.h"

namespace Poincare {

class Addition final : public NAry {
public:
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex, uint8_t numberOfChildren) { return NAry::CreateBlockAtIndex(block, blockIndex, numberOfChildren, AdditionBlock); }
  static float Reduce(float a, float b) { return a + b; }
};

}

#endif
