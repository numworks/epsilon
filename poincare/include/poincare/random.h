#ifndef POINCARE_RANDOM_H
#define POINCARE_RANDOM_H

#include <poincare/static_hierarchy.h>
#include <poincare/evaluation.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class Random : public StaticHierarchy<0> {
  using StaticHierarchy<0>::StaticHierarchy;
public:
  Type type() const override;
  Sign sign() const override { return Sign::Positive; }
  template<typename T> static T random();
private:
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override;
  /* Layout */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutHelper::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  const char * name() const { return "random"; }
  /* Evaluation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templateApproximate<float>();
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templateApproximate<double>();
  }
  template <typename T> Evaluation<T> templateApproximate()) const {
    return new Complex<T>(random<T>());
  }
};

}

#endif
