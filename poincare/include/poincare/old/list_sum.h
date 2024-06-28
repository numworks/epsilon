#ifndef POINCARE_LIST_SUM_H
#define POINCARE_LIST_SUM_H

#include "list.h"

namespace Poincare {

class ListSumNode : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "sum";
  size_t size() const override { return sizeof(ListSumNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "ListSum"; }
#endif
  Type otype() const override { return Type::ListSum; }
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

 private:
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  OExpression shallowReduce(const ReductionContext& reductionContext) override;
};

class ListSum : public ExpressionOneChild<ListSum, ListSumNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
