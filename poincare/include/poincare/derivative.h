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
  constexpr static float k_epsilon = 0.0001f;
  constexpr static float k_precision = 0.01f;
};

#endif
