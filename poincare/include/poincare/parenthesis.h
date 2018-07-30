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
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "");
  }
  /* Simplification */
  Expression * shallowReduce(Context& context, Preferences::AngleUnit angleUnit) override;
  /* Evaluation */
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> * templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;

};

}

#endif
