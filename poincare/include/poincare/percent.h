#ifndef POINCARE_PERCENT_H
#define POINCARE_PERCENT_H

#include <poincare/n_ary_expression.h>

namespace Poincare {

class PercentNode final : public NAryExpressionNode  {
public:
  // TreeNode
  size_t size() const override { return sizeof(PercentNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Percent";
  }
#endif
  // Properties
  Type type() const override { return Type::Percent; }
  bool childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const override;

private:
  // Layout
  bool childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplication
  Expression shallowReduce(ReductionContext reductionContext) override;
  Expression shallowBeautify(ReductionContext * reductionContext) override;
  LayoutShape leftLayoutShape() const override { return childAtIndex(0)->leftLayoutShape(); };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return templateApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return templateApproximate<double>(approximationContext);
  }
  template <typename T> Evaluation<T> templateApproximate(ApproximationContext approximationContext, bool * inputIsUndefined = nullptr) const;
};

class Percent final : public NAryExpression {
public:
  static Percent Builder(const Tuple & children = {}) { return TreeHandle::NAryBuilder<Percent, PercentNode>(convert(children)); }
  static Percent ParseTarget(Expression & leftHandSide);

  Percent(const PercentNode * n) : NAryExpression(n) {}
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  Expression shallowBeautify(ExpressionNode::ReductionContext * reductionContext);
};

}

#endif
