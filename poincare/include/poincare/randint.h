#ifndef POINCARE_RANDINT_H
#define POINCARE_RANDINT_H

#include <poincare/static_hierarchy.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class Randint : public StaticHierarchy<2> {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
private:
  /* Layout */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, name());
  }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  const char * name() const { return "randint"; }
  /* Evaluation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templateApproximate<float>(context, angleUnit);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templateApproximate<double>(context, angleUnit);
  }
  template <typename T> Evaluation<T> templateApproximate()Context& context, Preferences::AngleUnit angleUnit) const;
};

}

#endif
