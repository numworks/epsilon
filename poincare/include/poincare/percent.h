#ifndef POINCARE_PERCENT_H
#define POINCARE_PERCENT_H

#include <poincare/expression.h>

namespace Poincare {

namespace Percent {
  Expression ParseTarget(Expression & leftHandSide, bool isPercentSimple);
}

class PercentSimpleNode : public ExpressionNode  {
public:
  // TreeNode
  size_t size() const override { return sizeof(PercentSimpleNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Percent";
  }
#endif
  // Properties
  Type type() const override { return Type::PercentSimple; }
  Sign sign(Context * context) const override { return childAtIndex(0)->sign(context); }
  NullStatus nullStatus(Context * context) const override { return childAtIndex(0)->nullStatus(context); }
  bool childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const override;

protected:
  virtual int createSecondChildLayout(Poincare::HorizontalLayout * result, int childrenCount, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const { return childrenCount; }
  virtual int serializeSecondChild(char * buffer, int bufferSize, int numberOfChar, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const { return numberOfChar; }
  virtual Expression getExpression() const;

private:
  // Layout
  bool childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplication
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  Expression shallowBeautify(const ReductionContext& reductionContext) override;
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

class PercentAdditionNode final : public PercentSimpleNode  {
public:
  // TreeNode
  size_t size() const override { return sizeof(PercentAdditionNode); }
  int numberOfChildren() const override { return 2; }
  // Properties
  Type type() const override { return Type::PercentAddition; }
  Sign sign(Context * context) const override;
  NullStatus nullStatus(Context * context) const override;

private:
  // PercentSimpleNode
  int createSecondChildLayout(Poincare::HorizontalLayout * result, int childrenCount, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serializeSecondChild(char * buffer, int bufferSize, int numberOfChar, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  Expression getExpression() const override;
  // Simplication
  Expression shallowBeautify(const ReductionContext& reductionContext) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return templateApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return templateApproximate<double>(approximationContext);
  }
  template <typename U> Evaluation<U> templateApproximate(ApproximationContext approximationContext, bool * inputIsUndefined = nullptr) const;
};

class PercentSimple final : public ExpressionOneChild<PercentSimple, PercentSimpleNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowBeautify(const ExpressionNode::ReductionContext& reductionContext);
};

class PercentAddition final : public ExpressionTwoChildren<PercentAddition, PercentAdditionNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowBeautify(const ExpressionNode::ReductionContext& reductionContext);
  Expression deepBeautify(const ExpressionNode::ReductionContext& reductionContext);
};

}

#endif
