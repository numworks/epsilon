#ifndef POINCARE_NTH_ROOT_H
#define POINCARE_NTH_ROOT_H

#include <poincare/static_hierarchy.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class NthRoot : public StaticHierarchy<2>  {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
private:
  /* Layout */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "root");
  }
  /* Simplification */
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) override;
  /* Evaluation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;

};

}

#endif
