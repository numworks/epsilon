#ifndef POINCARE_ABSOLUTE_VALUE_H
#define POINCARE_ABSOLUTE_VALUE_H

#include <poincare/expression.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class AbsoluteValueNode final : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(AbsoluteValueNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "AbsoluteValue";
  }
#endif

  // Properties
  Type type() const override { return Type::AbsoluteValue; }
  Sign sign() const override { return Sign::Positive; }
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override;

  // Approximation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
    return Complex<T>(std::abs(c));
  }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;
};

class AbsoluteValue final : public Expression {
friend class AbsoluteValueNode;
public:
  AbsoluteValue(const AbsoluteValueNode * n) : Expression(n) {}
  static AbsoluteValue Builder(Expression child) { return AbsoluteValue(child); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("abs", 1, &UntypedBuilder);

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
private:
  explicit AbsoluteValue(Expression child) : Expression(TreePool::sharedPool()->createTreeNode<AbsoluteValueNode>()) {
    replaceChildAtIndexInPlace(0, child);
  }
  Expression setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
