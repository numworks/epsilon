#ifndef POINCARE_NTH_ROOT_H
#define POINCARE_NTH_ROOT_H

#include <poincare/static_hierarchy.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class NthRoot : public StaticHierarchy<2>  {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
private:
  /* Layout */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "root");
  }
  /* Simplification */
  ExpressionReference shallowReduce(Context& context, Preferences::AngleUnit angleUnit) override;
  /* Evaluation */
  EvaluationReference<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  EvaluationReference<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
 template<typename T> EvaluationReference<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;

};

}

#endif
