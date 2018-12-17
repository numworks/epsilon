#ifndef POINCARE_HYPERBOLIC_TANGENT_H
#define POINCARE_HYPERBOLIC_TANGENT_H

#include <poincare/approximation_helper.h>
#include <poincare/hyperbolic_trigonometric_function.h>

namespace Poincare {

class HyperbolicTangentNode final : public HyperbolicTrigonometricFunctionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(HyperbolicTangentNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "HyperbolicTangent";
  }
#endif

  // Properties
  Type type() const override { return Type::HyperbolicTangent; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  //Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class HyperbolicTangent final : public HyperbolicTrigonometricFunction {
public:
  HyperbolicTangent(const HyperbolicTangentNode * n) : HyperbolicTrigonometricFunction(n) {}
  static HyperbolicTangent Builder(Expression child) { return HyperbolicTangent(child); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("tanh", 1, &UntypedBuilder);
private:
  explicit HyperbolicTangent(Expression child) : HyperbolicTrigonometricFunction(TreePool::sharedPool()->createTreeNode<HyperbolicTangentNode>()) {
    replaceChildAtIndexInPlace(0, child);
  }
};

}

#endif
