#ifndef POINCARE_PARENTHESIS_H
#define POINCARE_PARENTHESIS_H

#include "complex_cartesian.h"
#include "old_expression.h"

namespace Poincare {

class ParenthesisNode final : public ExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(ParenthesisNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Parenthesis";
  }
#endif

  // Properties
  OMG::Troolean isPositive(Context* context) const override {
    return childAtIndex(0)->isPositive(context);
  }
  OMG::Troolean isNull(Context* context) const override {
    return childAtIndex(0)->isNull(context);
  }
  Type otype() const override { return Type::Parenthesis; }
  OExpression removeUnit(OExpression* unit) override {
    assert(false);
    return ExpressionNode::removeUnit(unit);
  }

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  };

  // Approximation
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

 private:
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
};

class Parenthesis final
    : public ExpressionOneChild<Parenthesis, ParenthesisNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  // OExpression
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
