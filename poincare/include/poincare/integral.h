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
  Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> * templatedEvaluate(Context& context, AngleUnit angleUnit) const;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  template<typename T>
  struct DetailedResult
  {
    T integral;
    T absoluteError;
  };
  constexpr static int k_maxNumberOfIterations = 100;
#ifdef LAGRANGE_METHOD
  template<typename T> T lagrangeGaussQuadrature(T a, T b, VariableContext<T> xContext, AngleUnit angleUnit) const;
#else
  template<typename T> DetailedResult<T> kronrodGaussQuadrature(T a, T b, VariableContext<T> xContext, AngleUnit angleUnit) const;
  template<typename T> T adaptiveQuadrature(T a, T b, T eps, int numberOfIterations, VariableContext<T> xContext, AngleUnit angleUnit) const;
#endif
  template<typename T> T functionValueAtAbscissa(T x, VariableContext<T> xcontext, AngleUnit angleUnit) const;
};

}

#endif
