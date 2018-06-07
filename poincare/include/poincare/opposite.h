#ifndef POINCARE_OPPOSITE_H
#define POINCARE_OPPOSITE_H

#include <poincare/static_hierarchy.h>
#include <poincare/layout_engine.h>
#include <poincare/approximation_engine.h>

namespace Poincare {

class Opposite : public StaticHierarchy<1> {
  using StaticHierarchy<1>::StaticHierarchy;
public:
  Expression * clone() const override;
  Type type() const override;
  int polynomialDegree(char symbolName) const override;
  Sign sign() const override;
  template<typename T> static std::complex<T> compute(const std::complex<T> c, AngleUnit angleUnit);
private:
  /* Layout */
  bool needParenthesisWithParent(const Expression * e) const override;
  ExpressionLayout * createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::map<float>(this, context, angleUnit, compute<float>);
  }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::map<double>(this, context, angleUnit, compute<double>);
  }
};

}

#endif
