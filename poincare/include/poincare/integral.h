#ifndef POINCARE_INTEGRAL_H
#define POINCARE_INTEGRAL_H

#include <poincare/function.h>
#include <poincare/variable_context.h>

class Integral : public Function {
public:
  Integral();
  float approximate(Context & context) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
  ExpressionLayout * createLayout() const override;
private:
  struct DetailedResult
  {
    float integral;
    float absoluteError;
  };
  constexpr static int k_maxNumberOfIterations = 100;
  //float lagrangeGaussQuadrature(float a, float b, VariableContext xContext) const;
  DetailedResult kronrodGaussQuadrature(float a, float b, VariableContext xContext) const;
  float adaptiveQuadrature(float a, float b, float eps, int numberOfIterations, VariableContext xContext) const;
  float functionValueAtAbscissa(float x, VariableContext xcontext) const;
};

#endif
