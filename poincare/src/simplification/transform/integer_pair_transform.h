#ifndef POINCARE_SIMPLIFICATION_INTEGER_PAIR_TRANSFORM_H
#define POINCARE_SIMPLIFICATION_INTEGER_PAIR_TRANSFORM_H

#include "transform.h"

namespace Poincare {

class IntegerPairTransform : public Transform {
public:
  constexpr IntegerPairTransform() {};
  void apply(Expression * root, Expression * captures[]) const override;
  virtual Integer process(const Integer & a, const Integer & b) const = 0;
};

}

#endif
