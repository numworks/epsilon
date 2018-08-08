#ifndef POINCARE_LOGARITHM_H
#define POINCARE_LOGARITHM_H

#include <poincare/layout_helper.h>
#include <poincare/bounded_static_hierarchy.h>
#include <poincare/integer.h>

namespace Poincare {

class Logarithm : public BoundedStaticHierarchy<2>  {
  using BoundedStaticHierarchy<2>::BoundedStaticHierarchy;
  friend class NaperianLogarithm;
public:
  Type type() const override;
private:
  /* Layout */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutHelper::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "log");
  }
  /* Simplification */
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  Expression * simpleShallowReduce(Context & context, Preferences::AngleUnit angleUnit);
  Expression * shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) override;
  bool parentIsAPowerOfSameBase() const;
  Expression * splitInteger(Integer i, bool isDenominator, Context & context, Preferences::AngleUnit angleUnit);
  /* Evaluation */
  template<typename T> static std::complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

}

#endif
