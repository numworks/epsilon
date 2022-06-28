#ifndef POINCARE_LIST_STANDARD_DEVIATION_H
#define POINCARE_LIST_STANDARD_DEVIATION_H

#include <poincare/list_two_parameters_function_node.h>

namespace Poincare {

class ListStandardDeviationNode : public ListTwoParametersFunctionNode {
public:
  constexpr static const char k_functionName[] = "stddev";
  const char * functionName() const override { return k_functionName; }

  size_t size() const override { return sizeof(ListStandardDeviationNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListStandardDeviation";
  }
#endif
  Type type() const override { return Type::ListStandardDeviation; }

private:
  Expression shallowReduce(const ReductionContext& reductionContext) override;

  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class ListStandardDeviation : public ExpressionTwoChildren<ListStandardDeviation, ListStandardDeviationNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext);
};

}

#endif
