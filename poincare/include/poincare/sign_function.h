#ifndef POINCARE_SIGN_FUNCTION_H
#define POINCARE_SIGN_FUNCTION_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class SignFunctionNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(SignFunctionNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "SignFunction";
  }
#endif

  // Properties
  Type type() const override { return Type::SignFunction; }
  Sign sign(Context * context) const override { return childAtIndex(0)->sign(context); }
  NullStatus nullStatus(Context * context) const override { return childAtIndex(0)->nullStatus(context); }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }
  // Derivation
  bool derivate(ReductionContext reductionContext, Symbol symbol, Expression symbolValue) override;
};

class SignFunction final : public Expression {
public:
  SignFunction(const SignFunctionNode * n) : Expression(n) {}
  static SignFunction Builder(Expression child) { return TreeHandle::FixedArityBuilder<SignFunction, SignFunctionNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("sign", 1, Initializer<SignFunctionNode>, sizeof(SignFunctionNode));

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  bool derivate(ExpressionNode::ReductionContext reductionContext, Symbol symbol, Expression symbolValue);
};

}

#endif
