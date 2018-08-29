#ifndef POINCARE_BINOMIAL_COEFFICIENT_H
#define POINCARE_BINOMIAL_COEFFICIENT_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>

namespace Poincare {

class BinomialCoefficient : public StaticHierarchy<2> {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
  template<typename T> static T compute(T k, T n);
private:
  constexpr static int k_maxNValue = 300;
  /* Layout */
  ExpressionLayout * createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, "binomial");
  }
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Complex<T> * templatedApproximate(Context& context, AngleUnit angleUnit) const;
};

}

#endif

