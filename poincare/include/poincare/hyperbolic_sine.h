#ifndef POINCARE_HYPERBOLIC_SINE_H
#define POINCARE_HYPERBOLIC_SINE_H

#include <poincare/function.h>

class HyperbolicSine : public Function {
public:
  HyperbolicSine();
  float approximate(Context & context, AngleUnit angleUnit = AngleUnit::Radian) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
  Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
};

#endif
