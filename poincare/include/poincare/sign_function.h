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
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "SignFunction";
  }
#endif

  // Properties
  Type type() const override { return Type::SignFunction; }
  Sign sign(Context * context) const override;
  Expression setSign(Sign s, Context * context, Preferences::AngleUnit angleUnit, ReductionTarget target) override;

  // Complex
  bool isReal(Context & context) const override { return true; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class SignFunction final : public Expression {
public:
  SignFunction(const SignFunctionNode * n) : Expression(n) {}
  static SignFunction Builder(Expression child) { return SignFunction(child); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("sign", 1, &UntypedBuilder);

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
private:
  explicit SignFunction(Expression child) : Expression(TreePool::sharedPool()->createTreeNode<SignFunctionNode>()) {
    replaceChildAtIndexInPlace(0, child);
  }
};

}

#endif
