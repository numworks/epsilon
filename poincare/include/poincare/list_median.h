#ifndef POINCARE_LIST_MEDIAN_H
#define POINCARE_LIST_MEDIAN_H

#include <poincare/list_function_with_up_to_two_parameters.h>

namespace Poincare {

class ListMedianNode : public ListFunctionWithOneOrTwoParametersNode {
public:
  constexpr static const char k_functionName[] = "med";
  const char * functionName() const override { return k_functionName; }

  size_t size() const override { return sizeof(ListMedianNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListMedian";
  }
#endif
  Type type() const override { return Type::ListMedian; }

private:
  Expression shallowReduce(const ReductionContext& reductionContext) override;

  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class ListMedian : public ExpressionUpToTwoChildren<ListMedian, ListMedianNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);

private:
  void approximationHelper(int * index1, int * index2, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

}

#endif
