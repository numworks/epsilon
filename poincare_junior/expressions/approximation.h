#ifndef POINCARE_EXPRESSIONS_APPROXIMATION_H
#define POINCARE_EXPRESSIONS_APPROXIMATION_H

#include "../type_block.h"

namespace Poincare {

class Approximation final {
public:
  typedef float (*Reductor)(float, float);
  static float MapAndReduce(const TypeBlock * block, Reductor reductor);
};

}

#endif
