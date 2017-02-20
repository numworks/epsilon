#ifndef POINCARE_INTEGRAL_H
#define POINCARE_INTEGRAL_H

#include <poincare/function.h>
#include <poincare/variable_context.h>

namespace Poincare {

class Integral : public Function {
public:
  Integral();
  float approximate(Context & context, AngleUnit angleUnit = AngleUnit::Radian) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
  ExpressionLayout * createLayout(FloatDisplayMode FloatDisplayMode = FloatDisplayMode::Auto) const override;
private:
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
