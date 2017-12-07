#ifndef POINCARE_NUMERICAL_BASE_H
#define POINCARE_NUMERICAL_BASE_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>
#include <poincare/complex.h>
#include <poincare/approximation_engine.h>

namespace Poincare {

class ToBase : public StaticHierarchy<2> {
public:
  using StaticHierarchy<2>::StaticHierarchy;
  Type type() const override;
  Expression * clone() const override;
  int polynomialDegree(char symbolName) const override;
private:
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Layout */
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
  /* Evaluation */
  template<typename T> static Complex<T> computeOnComplex(const Complex<T> c, AngleUnit angleUnit);
  Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return ApproximationEngine::map<float>(this, context, angleUnit, computeOnComplex<float>); }
  Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return ApproximationEngine::map<double>(this, context, angleUnit, computeOnComplex<double>); }
  template<typename T> Complex<T> * templatedApproximate(Context& context, AngleUnit angleUnit) const;
};

}

#endif

