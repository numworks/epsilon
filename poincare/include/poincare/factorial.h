#ifndef POINCARE_FACTORIAL_H
#define POINCARE_FACTORIAL_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class FactorialNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(FactorialNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Factorial";
  }
#endif

  // Properties
  NullStatus nullStatus(Context * context) const override { return NullStatus::NonNull; }
  Type type() const override { return Type::Factorial; }
  Sign sign(Context * context) const override { return Sign::Positive; }
  bool childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const override;

private:
  // Layout
  bool childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplication
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return childAtIndex(0)->leftLayoutShape(); };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }

#if 0
  int simplificationOrderGreaterType(const Expression e) const override;
  int simplificationOrderSameType(const Expression e) const override;
#endif
};

class Factorial final : public ExpressionOneChild<Factorial, FactorialNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
private:
  constexpr static int k_maxOperandValue = 100;
};

}

#endif
