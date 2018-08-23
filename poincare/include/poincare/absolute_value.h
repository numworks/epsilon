#ifndef POINCARE_ABSOLUTE_VALUE_H
#define POINCARE_ABSOLUTE_VALUE_H

#include <poincare/static_hierarchy.h>
#include <poincare/approximation_helper.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class AbsoluteValue : public StaticHierarchy<1> {
  using StaticHierarchy<1>::StaticHierarchy;
public:
  Type type() const override;
  Sign sign() const override { return Sign::Positive; }
private:
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) const override;
  /* Layout */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "abs");
  }
  /* Simplification */
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const override;
  /* Evaluation */
  template<typename T> static std::complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
  return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

}

#endif
