#ifndef POINCARE_SIMPLIFICATION_TRANSFORM_SUBTRACTION_TRANSFORM_H
#define POINCARE_SIMPLIFICATION_TRANSFORM_SUBTRACTION_TRANSFORM_H

#include "../transform.h"

namespace Poincare {

class SubtractionTransform : public Transform {
public:
  constexpr SubtractionTransform() {};
  void apply(Expression * root, Expression * captures[]) const override;
};

}

#endif
