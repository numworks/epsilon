#ifndef POINCARE_PIECEWISE_OPERATOR_H
#define POINCARE_PIECEWISE_OPERATOR_H

#include <limits.h>

#include "old_expression.h"

/* The syntax is piecewise(result1,condition1,result2,condtion2,...,resultN)
 * There can be a final resultN without condition or not.
 * If a condition is undef or if every condition is false and there is no final
 * resultN, piecewise = undef. */
namespace Poincare {

class PiecewiseOperatorNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "piecewise";

  // PoolObject
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
  Type otype() const override { return Type::PiecewiseOperator; }
  template <typename T>
  int indexOfFirstTrueCondition(
      const ApproximationContext& approximationContext) const;

  // Derivation
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::Default; };

  int m_numberOfChildren;
};

class PiecewiseOperator final : public OExpression {
  friend class OExpression;

 public:
  PiecewiseOperator(const PiecewiseOperatorNode* n) : OExpression(n) {}
  using PoolHandle::addChildAtIndexInPlace;
  using PoolHandle::removeChildAtIndexInPlace;
  static OExpression UntypedBuilder(OExpression children);
  constexpr static OExpression::FunctionHelper s_functionHelper =
      OExpression::FunctionHelper("piecewise", 1, INT_MAX, &UntypedBuilder);

  // OExpression
  OExpression shallowReduce(ReductionContext reductionContext);
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);

  // Returns -1 if every condition is false
  template <typename T>
  int indexOfFirstTrueConditionWithValueForSymbol(
      const char* symbol, T x,
      const ApproximationContext& approximationContext) const;

 private:
  static PiecewiseOperator Builder(const Tuple& children) {
    return PoolHandle::NAryBuilder<PiecewiseOperator, PiecewiseOperatorNode>(
        convert(children));
  }
  // This will shallowReduce the resulting expression.
  OExpression bubbleUpPiecewiseDependencies(
      const ReductionContext& reductionContext);
};

}  // namespace Poincare

#endif
