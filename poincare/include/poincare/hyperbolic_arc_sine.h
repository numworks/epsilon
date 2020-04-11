#ifndef POINCARE_HYPERBOLIC_ARC_SINE_H
#define POINCARE_HYPERBOLIC_ARC_SINE_H

#include <poincare/approximation_helper.h>
#include <poincare/hyperbolic_trigonometric_function.h>

namespace Poincare {

class HyperbolicArcSineNode final : public HyperbolicTrigonometricFunctionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(HyperbolicArcSineNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "HyperbolicArcSine";
  }
#endif

  // Properties
  Type type() const override { return Type::HyperbolicArcSine; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  //Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, complexFormat, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, complexFormat, angleUnit, computeOnComplex<double>);
  }
};

class HyperbolicArcSine final : public HyperbolicTrigonometricFunction {
public:
  HyperbolicArcSine(const HyperbolicArcSineNode * n) : HyperbolicTrigonometricFunction(n) {}
  static HyperbolicArcSine Builder(Expression child) { return TreeHandle::FixedArityBuilder<HyperbolicArcSine, HyperbolicArcSineNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("asinh", 1, &UntypedBuilderOneChild<HyperbolicArcSine>);
};

}


#endif
