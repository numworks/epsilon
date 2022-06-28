#ifndef POINCARE_LIST_MEAN_H
#define POINCARE_LIST_MEAN_H

#include <poincare/list_two_parameters_function_node.h>

namespace Poincare {

class ListMeanNode : public ListTwoParametersFunctionNode {
public:
  constexpr static char k_functionName[] = "mean";
  const char * functionName() const override { return k_functionName; }

  size_t size() const override { return sizeof(ListMeanNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListMean" ;
  }
#endif
  Type type() const override { return Type::ListMean; }

private:
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
