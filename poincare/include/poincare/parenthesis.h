#ifndef POINCARE_PARENTHESIS_H
#define POINCARE_PARENTHESIS_H

#include <poincare/static_hierarchy.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class Parenthesis : public StaticHierarchy<1> {
public:
  using StaticHierarchy<1>::StaticHierarchy;
public:
  Expression * clone() const override;
  Type type() const override;
  int polynomialDegree(char symbolName) const override;
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, "");
  }
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
 template<typename T> Expression * templatedApproximate(Context& context, AngleUnit angleUnit) const;

};

}

#endif
