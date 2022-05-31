#ifndef POINCARE_PERCENT_H
#define POINCARE_PERCENT_H

#include <poincare/expression.h>

namespace Poincare {

template<int T>
class PercentNode final : public ExpressionNode  {
public:
  // TreeNode
  size_t size() const override { return sizeof(PercentNode); }
  int numberOfChildren() const override { return T; }
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
  Expression deepBeautify(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return childAtIndex(0)->leftLayoutShape(); }
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return templateApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return templateApproximate<double>(approximationContext);
  }
  template <typename U> Evaluation<U> templateApproximate(ApproximationContext approximationContext, bool * inputIsUndefined = nullptr) const;
};

class Percent : public Expression {
public:
  static Expression ParseTarget(Expression & leftHandSide);

  template <int T> Percent(const PercentNode<T> * n) : Expression(n) {}
  Percent(const PercentNode<2> * n) : Expression(n) {}
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

class PercentAddition final : public Percent {
public:
  static PercentAddition Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<PercentAddition, PercentNode<2>>({child0, child1}); }

  PercentAddition(const PercentNode<2> * n) : Percent(n) {}
  Expression shallowBeautify(ExpressionNode::ReductionContext * reductionContext);
  Expression deepBeautify(ExpressionNode::ReductionContext reductionContext);
};

class PercentSimple final : public Percent {
public:
  static PercentSimple Builder(Expression child) { return TreeHandle::FixedArityBuilder<PercentSimple, PercentNode<1>>({child}); }

  PercentSimple(const PercentNode<1> * n) : Percent(n) {}
  Expression shallowBeautify(ExpressionNode::ReductionContext * reductionContext);
};

}

#endif
