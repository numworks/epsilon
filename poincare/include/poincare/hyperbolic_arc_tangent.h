#ifndef POINCARE_HYPERBOLIC_ARC_TANGENT_H
#define POINCARE_HYPERBOLIC_ARC_TANGENT_H

#include <poincare/approximation_helper.h>
#include <poincare/hyperbolic_trigonometric_function.h>

namespace Poincare {

class HyperbolicArcTangentNode final : public HyperbolicTrigonometricFunctionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(HyperbolicArcTangentNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "HyperbolicArcTangent";
  }
#endif

  // Properties
  Type type() const override { return Type::HyperbolicArcTangent; }
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

class HyperbolicArcTangent final : public HyperbolicTrigonometricFunction {
public:
  HyperbolicArcTangent(const HyperbolicArcTangentNode * n) : HyperbolicTrigonometricFunction(n) {}
  static HyperbolicArcTangent Builder(Expression child) { return TreeHandle::FixedArityBuilder<HyperbolicArcTangent, HyperbolicArcTangentNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("atanh", 1, &UntypedBuilderOneChild<HyperbolicArcTangent>);
};

}

#endif
