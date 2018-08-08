#ifndef POINCARE_SINE_H
#define POINCARE_SINE_H

#include <poincare/layout_helper.h>
#include <poincare/static_hierarchy.h>
#include <poincare/approximation_helper.h>
#include <poincare/trigonometry.h>

namespace Poincare {

class Sine : public StaticHierarchy<1> {
  using StaticHierarchy<1>::StaticHierarchy;
  friend class Tangent;
  float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const override;
public:
  Type type() const override;
  template<typename T> static std::complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Radian);
private:
  /* Layout */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, name());
  }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutHelper::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  const char * name() const { return "sin"; }
  /* Simplication */
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) override;
  /* Evaluation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

}

#endif
