#ifndef POINCARE_ROUND_H
#define POINCARE_ROUND_H

#include <poincare/expression.h>
#include <poincare/expression_node_with_up_to_two_children.h>
#include <poincare/evaluation.h>

namespace Poincare {

class RoundNode final : public ExpressionNodeWithOneOrTwoChildren  {
public:
  constexpr static AliasesList k_functionName = "round";

  // TreeNode
  size_t size() const override { return sizeof(RoundNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Round";
  }
#endif


  // Properties
  TrinaryBoolean isPositive(Context * context) const override { return childAtIndex(0)->isPositive(context); }
  Type type() const override { return Type::Round; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class Round final : public ExpressionUpToTwoChildren<Round, RoundNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
