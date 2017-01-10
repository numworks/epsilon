#ifndef POINCARE_DERIVATIVE_H
#define POINCARE_DERIVATIVE_H

#include <poincare/function.h>

class Derivative : public Function {
public:
  Derivative();
  float approximate(Context & context) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
private:
// float precision ~ 2^-23 -> we chose eps ~ precision^(1/3) because (f(x+eps)-f(x-eps))/(2eps) < (precision/h)*f(x)
  constexpr static float k_epsilon = 0.004921566601f;
};

#endif
