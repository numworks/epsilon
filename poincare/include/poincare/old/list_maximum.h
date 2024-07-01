#ifndef POINCARE_LIST_MAXIMUM_H
#define POINCARE_LIST_MAXIMUM_H

#include "old_expression.h"

namespace Poincare {

class ListMaximumNode : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "max";
  size_t size() const override { return sizeof(ListMaximumNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ListMaximum";
  }
#endif
  Type otype() const override { return Type::ListMaximum; }
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

class ListMaximum : public ExpressionOneChild<ListMaximum, ListMaximumNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
