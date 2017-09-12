#ifndef POINCARE_SIMPLIFY_INTEGER_ADDITION_H
#define POINCARE_SIMPLIFY_INTEGER_ADDITION_H

#include "transform.h"

namespace Poincare {

class IntegerAddition : public Transform {
public:
  constexpr IntegerAddition() {};
  void apply(Expression * root, Expression * captures[]) const override;
};

}

#endif
