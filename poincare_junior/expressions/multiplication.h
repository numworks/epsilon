#ifndef POINCARE_EXPRESSIONS_MULTIPLICATION_H
#define POINCARE_EXPRESSIONS_MULTIPLICATION_H

#include "n_ary.h"

namespace Poincare {

class Multiplication final : public NAry {
public:
   constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex, uint8_t numberOfChildren) { return NAry::CreateBlockAtIndex(block, blockIndex, numberOfChildren, MultiplicationBlock); }
  static float Reduce(float a, float b) { return a * b; }
  static TypeBlock * DistributeOverAddition(TypeBlock * block);
};

}

#endif
