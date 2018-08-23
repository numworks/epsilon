#ifndef POINCARE_FACTORIAL_H
#define POINCARE_FACTORIAL_H

#include <poincare/static_hierarchy.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class Factorial : public StaticHierarchy<1> {
public:
  Factorial(const Expression * argument, bool clone = true);
  Type type() const override;
private:
  constexpr static int k_maxOperandValue = 100;
  /* Layout */
  bool needsParenthesesWithParent(SerializableNode * parentNode) const override;
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Simplication */
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const override;
  Expression shallowBeautify(Context& context, Preferences::AngleUnit angleUnit) override;
  /* Evaluation */
  template<typename T> static std::complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }

#if 0
  int simplificationOrderGreaterType(const Expression * e) const override;
  int simplificationOrderSameType(const Expression * e) const override;
#endif
};

}

#endif
