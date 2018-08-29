#ifndef POINCARE_FACTOR_H
#define POINCARE_FACTOR_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/complex.h>
#include <cmath>

namespace Poincare {

class Factor : public StaticHierarchy<1> {
  using StaticHierarchy<1>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, complexFormat, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, name());
  }
  const char * name() const { return "factor"; }
  /* Simplification */
  Expression * shallowBeautify(Context& context, AngleUnit angleUnit) override;
  Expression * createMultiplicationOfIntegerPrimeDecomposition(Integer i, Context & context, AngleUnit angleUnit);
  /* Evaluation */
  Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Expression * templatedApproximate(Context& context, AngleUnit angleUnit) const {
    return operand(0)->approximate<T>(context, angleUnit);
  }
};

}

#endif

