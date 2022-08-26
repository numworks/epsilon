#ifndef POINCARE_LIST_VARIANCE_H
#define POINCARE_LIST_VARIANCE_H

#include <poincare/list_function_with_up_to_two_parameters.h>

namespace Poincare {

class ListVarianceNode : public ListFunctionWithOneOrTwoParametersNode {
public:
  constexpr static const char k_functionName[] = "var";
  const char * functionName() const override { return k_functionName; }

  size_t size() const override { return sizeof(ListVarianceNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListVariance";
  }
#endif
  Type type() const override { return Type::ListVariance; }

private:
  Expression shallowReduce(const ReductionContext& reductionContext) override;

  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class ListVariance : public ExpressionUpToTwoChildren<ListVariance, ListVarianceNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
