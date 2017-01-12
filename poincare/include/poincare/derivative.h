#ifndef POINCARE_DERIVATIVE_H
#define POINCARE_DERIVATIVE_H

#include <poincare/function.h>
#include <poincare/x_context.h>

class Derivative : public Function {
public:
  Derivative();
  float approximate(Context & context) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
private:
  float growthRateAroundAbscissa(float x, float h, XContext xContext) const;
  float approximateDerivate2(float x, float h, XContext xContext) const;
  constexpr static float k_maxErrorRateOnApproximation = 0.001f;
  constexpr static float k_minInitialRate = 0.01f;
  constexpr static float k_rateStepSize = 1.4f;
};

#endif
