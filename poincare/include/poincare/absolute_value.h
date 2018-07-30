#ifndef POINCARE_ABSOLUTE_VALUE_H
#define POINCARE_ABSOLUTE_VALUE_H

#include <poincare/static_hierarchy.h>
#include <poincare/approximation_engine.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class AbsoluteValue : public StaticHierarchy<1> {
  using StaticHierarchy<1>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
  Sign sign() const override { return Sign::Positive; }
private:
  Expression * setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override;
  /* Layout */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "abs");
  }
  /* Simplification */
  Expression * shallowReduce(Context& context, Preferences::AngleUnit angleUnit) override;
  /* Evaluation */
  template<typename T> static std::complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationEngine::map<float>(this, context, angleUnit, computeOnComplex<float>);
  }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
  return ApproximationEngine::map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

}

#endif
