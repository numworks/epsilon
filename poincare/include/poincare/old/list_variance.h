#ifndef POINCARE_LIST_VARIANCE_H
#define POINCARE_LIST_VARIANCE_H

#include "list_function_with_up_to_two_parameters.h"

namespace Poincare {

class ListVarianceNode : public ListFunctionWithOneOrTwoParametersNode {
 public:
  constexpr static const char k_functionName[] = "var";
  const char* functionName() const override { return k_functionName; }

  size_t size() const override { return sizeof(ListVarianceNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ListVariance";
  }
#endif
  Type otype() const override { return Type::ListVariance; }

 private:
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
};

class ListVariance
    : public ExpressionUpToTwoChildren<ListVariance, ListVarianceNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
