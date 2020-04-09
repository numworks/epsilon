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
  Type type() const override { return Type::Factorial; }
  Sign sign(Context * context) const override { return Sign::Positive; }
  Expression setSign(Sign s, ReductionContext reductionContext) override;
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
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, complexFormat, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, complexFormat, angleUnit, computeOnComplex<double>);
  }

#if 0
  int simplificationOrderGreaterType(const Expression e) const override;
  int simplificationOrderSameType(const Expression e) const override;
#endif
};

class Factorial final : public Expression {
public:
  Factorial(const FactorialNode * n) : Expression(n) {}
  static Factorial Builder(Expression child) { return TreeHandle::FixedArityBuilder<Factorial, FactorialNode>({child}); }

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
private:
  constexpr static int k_maxOperandValue = 100;
};

}

#endif
