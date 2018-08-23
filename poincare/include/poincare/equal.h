#ifndef POINCARE_EQUAL_H
#define POINCARE_EQUAL_H

#include <poincare/expression.h>
#include <poincare/static_hierarchy.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class Equal : public StaticHierarchy<2> {
public:
  using StaticHierarchy<2>::StaticHierarchy;
  Type type() const override;
  int polynomialDegree(char symbolName) const override;
  // For the equation A = B, create the reduced expression A-B
  Expression * standardEquation(Context & context, Preferences::AngleUnit angleUnit) const;
private:
  /* Simplification */
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const override;
  /* Layout */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "=");
  }
  /* Evalutation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

}

#endif
