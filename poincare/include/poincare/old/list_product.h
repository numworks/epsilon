#ifndef POINCARE_LIST_PRODUCT_H
#define POINCARE_LIST_PRODUCT_H

#include "old_expression.h"

namespace Poincare {

class ListProductNode : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "prod";
  size_t size() const override { return sizeof(ListProductNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ListProduct";
  }
#endif
  Type otype() const override { return Type::ListProduct; }
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

class ListProduct : public ExpressionOneChild<ListProduct, ListProductNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
