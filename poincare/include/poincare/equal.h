#ifndef POINCARE_EQUAL_H
#define POINCARE_EQUAL_H

#include <poincare/expression.h>
#include <poincare/static_hierarchy.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class Equal : public StaticHierarchy<2> {
public:
  using StaticHierarchy<2>::StaticHierarchy;
  Type type() const override;
  Expression * clone() const override;
  int polynomialDegree(char symbolName) const override;
  // For the equation A = B, create the reduced expression A-B
  Expression * standardEquation(Context & context, AngleUnit angleUnit = AngleUnit::Default) const;
private:
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Layout */
  LayoutRef privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, "=");
  }
  /* Evalutation */
  Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Expression * templatedApproximate(Context& context, AngleUnit angleUnit) const;
};

}

#endif
