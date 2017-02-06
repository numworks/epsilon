#ifndef POINCARE_HYPERBOLIC_TANGENT_H
#define POINCARE_HYPERBOLIC_TANGENT_H

#include <poincare/function.h>

namespace Poincare {

class HyperbolicTangent : public Function {
public:
  HyperbolicTangent();
  float approximate(Context & context, AngleUnit angleUnit = AngleUnit::Radian) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
  Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
};

}

#endif
