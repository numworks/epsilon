#ifndef POINCARE_LIST_SORT_H
#define POINCARE_LIST_SORT_H

#include "old_expression.h"

namespace Poincare {

class ListSortNode : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "sort";
  size_t size() const override { return sizeof(ListSortNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ListSort";
  }
#endif
  Type otype() const override { return Type::ListSort; }
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

  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
};

class ListSort : public ExpressionOneChild<ListSort, ListSortNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
