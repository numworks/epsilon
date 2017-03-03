#ifndef POINCARE_INTEGRAL_H
#define POINCARE_INTEGRAL_H

#include <poincare/function.h>
#include <poincare/variable_context.h>

namespace Poincare {

class Integral : public Function {
public:
  Integral();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
private:
  float privateApproximate(Context & context, AngleUnit angleUnit) const override;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  struct DetailedResult
  {
    float integral;
    float absoluteError;
  };
  constexpr static int k_maxNumberOfIterations = 100;
#ifdef LAGRANGE_METHOD
  float lagrangeGaussQuadrature(float a, float b, VariableContext xContext, AngleUnit angleUnit) const;
#else
  DetailedResult kronrodGaussQuadrature(float a, float b, VariableContext xContext, AngleUnit angleUnit) const;
  float adaptiveQuadrature(float a, float b, float eps, int numberOfIterations, VariableContext xContext, AngleUnit angleUnit) const;
#endif
  float functionValueAtAbscissa(float x, VariableContext xcontext, AngleUnit angleUnit) const;
};

}

#endif
