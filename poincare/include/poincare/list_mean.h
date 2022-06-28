#ifndef POINCARE_LIST_MEAN_H
#define POINCARE_LIST_MEAN_H

#include <poincare/list.h>

namespace Poincare {

class ListMeanNode : public ExpressionNode {
public:
  constexpr static char k_functionName[] = "mean";

  size_t size() const override { return sizeof(ListMeanNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListMean" ;
  }
#endif
  Type type() const override { return Type::ListMean; }
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

  bool canTakeDefaultParameterAtIndex(int i) const override { return i > 0; }
  Expression defaultParameterAtIndex(int i) const override;

private:
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  Expression shallowReduce(const ReductionContext& reductionContext) override;

  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class ListMean : public ExpressionTwoChildren<ListMean, ListMeanNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext);
};

}

#endif
