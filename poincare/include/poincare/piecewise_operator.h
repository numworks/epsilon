#ifndef POINCARE_PIECEWISE_OPERATOR_H
#define POINCARE_PIECEWISE_OPERATOR_H

#include <limits.h>
#include <poincare/expression.h>

/* The syntax is piecewise(result1,condition1,result2,condtion2,...,resultN)
 * There can be a final resultN without condition or not.
 * If a condition is undef or if every condition is false and there is no final
 * resultN, piecewise = undef. */
namespace Poincare {

class PiecewiseOperatorNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "piecewise";

  // TreeNode
  size_t size() const override { return sizeof(PiecewiseOperatorNode); }
  int numberOfChildren() const override { return m_numberOfChildren; }
  void setNumberOfChildren(int numberOfChildren) override {
    m_numberOfChildren = numberOfChildren;
  }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "PiecewiseOperator";
  }
#endif

  // Properties
  Type type() const override { return Type::PiecewiseOperator; }
  template <typename T>
  int indexOfFirstTrueCondition(
      const ApproximationContext& approximationContext) const;

 private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::Default; };
  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;

  int m_numberOfChildren;
};

class PiecewiseOperator final : public Expression {
  friend class Expression;

 public:
  PiecewiseOperator(const PiecewiseOperatorNode* n) : Expression(n) {}
  using TreeHandle::addChildAtIndexInPlace;
  static Expression UntypedBuilder(Expression children);
  constexpr static Expression::FunctionHelper s_functionHelper =
      Expression::FunctionHelper("piecewise", 1, INT_MAX, &UntypedBuilder);

  // Expression
  Expression shallowReduce(ReductionContext reductionContext);

  // Returns -1 if every condition is false
  template <typename T>
  int indexOfFirstTrueConditionWithValueForSymbol(
      const char* symbol, T x, Context* context,
      Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit) const;

 private:
  static PiecewiseOperator Builder(const Tuple& children) {
    return TreeHandle::NAryBuilder<PiecewiseOperator, PiecewiseOperatorNode>(
        convert(children));
  }
};

}  // namespace Poincare

#endif
