#ifndef POINCARE_INTEGRAL_H
#define POINCARE_INTEGRAL_H

#include <poincare/static_hierarchy.h>
#include <poincare/variable_context.h>
#include <poincare/layout_engine.h>
#include <poincare/complex.h>

namespace Poincare {

class Integral : public StaticHierarchy<3> {
  using StaticHierarchy<3>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, "int");
  }
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  Expression * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Expression * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
 template<typename T> Complex<T> * templatedEvaluate(Context& context, AngleUnit angleUnit) const;
  template<typename T>
  struct DetailedResult
  {
    T integral;
    T absoluteError;
  };
  constexpr static int k_maxNumberOfIterations = 10;
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
