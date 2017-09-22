#ifndef POINCARE_SIMPLIFICATION_TRANSFORM_INTEGER_ADDITION_TRANSFORM_H
#define POINCARE_SIMPLIFICATION_TRANSFORM_INTEGER_ADDITION_TRANSFORM_H

#include "transform.h"

namespace Poincare {
namespace Simplification {

class IntegerAdditionTransform : public Transform {
public:
  constexpr IntegerAdditionTransform() {};
  void apply(Expression * root, Expression * captures[]) const override;
};

}
}

#endif
