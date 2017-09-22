#ifndef POINCARE_SIMPLIFICATION_TRANSFORM_INTEGER_ADDITION_H
#define POINCARE_SIMPLIFICATION_TRANSFORM_INTEGER_ADDITION_H

#include "transform.h"

namespace Poincare {
namespace Simplification {

class IntegerAddition : public Transform {
public:
  constexpr IntegerAddition() {};
  void apply(Expression * root, Expression * captures[]) const override;
};

}
}

#endif
