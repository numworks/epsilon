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

  // Derivation
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue) override;

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
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
  using TreeHandle::removeChildAtIndexInPlace;
  static Expression UntypedBuilder(Expression children);
  constexpr static Expression::FunctionHelper s_functionHelper =
      Expression::FunctionHelper("piecewise", 1, INT_MAX, &UntypedBuilder);

  // Expression
  Expression shallowReduce(ReductionContext reductionContext);
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue);

  // Returns -1 if every condition is false
  template <typename T>
  int indexOfFirstTrueConditionWithValueForSymbol(
      const char* symbol, T x,
      const ApproximationContext& approximationContext) const;

 private:
  static PiecewiseOperator Builder(const Tuple& children) {
    return TreeHandle::NAryBuilder<PiecewiseOperator, PiecewiseOperatorNode>(
        convert(children));
  }
  // This will shallowReduce the resulting expression.
  Expression bubbleUpPiecewiseDependencies(
      const ReductionContext& reductionContext);
};

}  // namespace Poincare

#endif
