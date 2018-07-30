#ifndef POINCARE_DIVISION_REMAINDER_H
#define POINCARE_DIVISION_REMAINDER_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>
#include <poincare/evaluation.h>

namespace Poincare {

class DivisionRemainder : public StaticHierarchy<2> {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
private:
  /* Layout */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, numberOfSignificantDigits, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  const char * name() const { return "rem"; }
  /* Simplification */
  Expression * shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  /* Evaluation */
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Complex<T> * templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

}

#endif

