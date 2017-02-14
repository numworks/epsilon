#ifndef POINCARE_DERIVATIVE_H
#define POINCARE_DERIVATIVE_H

#include <poincare/function.h>
#include <poincare/variable_context.h>

namespace Poincare {

class Derivative : public Function {
public:
  Derivative();
  float approximate(Context & context, AngleUnit angleUnit = AngleUnit::Radian) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
private:
  float growthRateAroundAbscissa(float x, float h, VariableContext variableContext, AngleUnit angleUnit) const;
  float approximateDerivate2(float x, float h, VariableContext xContext, AngleUnit angleUnit) const;
  constexpr static float k_maxErrorRateOnApproximation = 0.001f;
  constexpr static float k_minInitialRate = 0.01f;
  constexpr static float k_rateStepSize = 1.4f;
};

}

#endif
