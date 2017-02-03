#ifndef POINCARE_HYPERBOLIC_COSINE_H
#define POINCARE_HYPERBOLIC_COSINE_H

#include <poincare/function.h>

class HyperbolicCosine : public Function {
public:
  HyperbolicCosine();
  float approximate(Context & context, AngleUnit angleUnit = AngleUnit::Radian) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
  Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
};

#endif
