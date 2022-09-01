#ifndef POINCARE_PERCENT_H
#define POINCARE_PERCENT_H

#include <poincare/expression.h>

namespace Poincare {

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
  TrinaryBoolean isPositive(Context * context) const override { return childAtIndex(0)->isPositive(context); }
  TrinaryBoolean isNull(Context * context) const override { return childAtIndex(0)->isNull(context); }
  bool childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const override;

protected:
  virtual int createSecondChildLayout(Poincare::HorizontalLayout * result, int childrenCount, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const { return childrenCount; }
  virtual int serializeSecondChild(char * buffer, int bufferSize, int numberOfChar, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const { return numberOfChar; }

private:
  // Layout
  bool childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplication
  Expression shallowBeautify(const ReductionContext& reductionContext) override;
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return childAtIndex(0)->leftLayoutShape(); }
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override {
    return templateApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override {
    return templateApproximate<double>(approximationContext);
  }
  template <typename U> Evaluation<U> templateApproximate(const ApproximationContext& approximationContext, bool * inputIsUndefined = nullptr) const;
};

class PercentAdditionNode final : public PercentSimpleNode  {
public:
  // TreeNode
  size_t size() const override { return sizeof(PercentAdditionNode); }
  int numberOfChildren() const override { return 2; }
  // Properties
  Type type() const override { return Type::PercentAddition; }
  TrinaryBoolean isPositive(Context * context) const override;
  TrinaryBoolean isNull(Context * context) const override;

private:
  // PercentSimpleNode
  int createSecondChildLayout(Poincare::HorizontalLayout * result, int childrenCount, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serializeSecondChild(char * buffer, int bufferSize, int numberOfChar, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplication
  Expression shallowBeautify(const ReductionContext& reductionContext) override;
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override {
    return templateApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override {
    return templateApproximate<double>(approximationContext);
  }
  template <typename U> Evaluation<U> templateApproximate(const ApproximationContext& approximationContext, bool * inputIsUndefined = nullptr) const;
};

class PercentSimple : public ExpressionOneChild<PercentSimple, PercentSimpleNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowBeautify(const ExpressionNode::ReductionContext& reductionContext);
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

class PercentAddition final : public ExpressionTwoChildren<PercentAddition, PercentAdditionNode, PercentSimple> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowBeautify(const ExpressionNode::ReductionContext& reductionContext);
  Expression deepBeautify(const ExpressionNode::ReductionContext& reductionContext);
};

}

#endif
