#ifndef POINCARE_ROUND_H
#define POINCARE_ROUND_H

#include <poincare/expression.h>
#include <poincare/evaluation.h>

namespace Poincare {

class RoundNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(RoundNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Round";
  }
#endif


  // Properties
  Sign sign(Context * context) const override { return childAtIndex(0)->sign(context); }
  Type type() const override { return Type::Round; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class Round final : public Expression {
public:
  Round(const RoundNode * n) : Expression(n) {}
  static Round Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<Round, RoundNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("round", 2, Initializer<RoundNode>, sizeof(RoundNode));

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
