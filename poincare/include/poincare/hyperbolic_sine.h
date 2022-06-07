#ifndef POINCARE_HYPERBOLIC_SINE_H
#define POINCARE_HYPERBOLIC_SINE_H

#include <poincare/approximation_helper.h>
#include <poincare/hyperbolic_trigonometric_function.h>

namespace Poincare {

class HyperbolicSineNode final : public HyperbolicTrigonometricFunctionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(HyperbolicSineNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "HyperbolicSine";
  }
#endif

  // Properties
  Type type() const override { return Type::HyperbolicSine; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Derivation
  bool derivate(ReductionContext reductionContext, Symbol symbol, Expression symbolValue) override;
  Expression unaryFunctionDifferential(ReductionContext reductionContext) override;
  //Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }
};

class HyperbolicSine final : public HyperbolicTrigonometricFunction {
public:
  HyperbolicSine(const HyperbolicSineNode * n) : HyperbolicTrigonometricFunction(n) {}
  static HyperbolicSine Builder(Expression child) { return TreeHandle::FixedArityBuilder<HyperbolicSine, HyperbolicSineNode>({child}); }

  bool derivate(ExpressionNode::ReductionContext reductionContext, Symbol symbol, Expression symbolValue);
  Expression unaryFunctionDifferential(ExpressionNode::ReductionContext reductionContext);

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("sinh", 1, Initializer<HyperbolicSineNode>, sizeof(HyperbolicSineNode));
};

}

#endif
