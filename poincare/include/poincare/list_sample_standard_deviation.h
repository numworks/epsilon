#ifndef POINCARE_LIST_SAMPLE_STANDARD_DEVIATION_H
#define POINCARE_LIST_SAMPLE_STANDARD_DEVIATION_H

#include <poincare/list_function_with_up_to_two_parameters.h>

namespace Poincare {

class ListSampleStandardDeviationNode : public ListFunctionWithOneOrTwoParametersNode {
public:
  constexpr static char k_functionName[] = "samplestddev";
  const char * functionName() const override { return k_functionName; }

  size_t size() const override { return sizeof(ListSampleStandardDeviationNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListSampleStandardDeviation";
  }
#endif
  Type type() const override { return Type::ListSampleStandardDeviation; }

private:
  Expression shallowReduce(const ReductionContext& reductionContext) override;

  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class ListSampleStandardDeviation : public ExpressionUpToTwoChildren<ListSampleStandardDeviation, ListSampleStandardDeviationNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
