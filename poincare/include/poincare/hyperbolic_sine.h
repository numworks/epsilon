#ifndef POINCARE_HYPERBOLIC_SINE_H
#define POINCARE_HYPERBOLIC_SINE_H

#include <poincare/function.h>

namespace Poincare {

class HyperbolicSine : public Function {
public:
  HyperbolicSine();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  float privateApproximate(Context & context, AngleUnit angleUnit) const override;
  Expression * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
};

}

#endif
