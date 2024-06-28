#ifndef POINCARE_SUBSTRACTION_H
#define POINCARE_SUBSTRACTION_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class SubtractionNode final : public ExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(SubtractionNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Subtraction";
  }
#endif

  // ExpressionNode

  // Properties
  Type otype() const override { return Type::Subtraction; }
  bool childAtIndexNeedsUserParentheses(const OExpression& child,
                                        int childIndex) const override;
  OExpression removeUnit(OExpression* unit) override {
    assert(false);
    return ExpressionNode::removeUnit(unit);
  }

  /* Layout */
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  /* Simplification */
  OExpression shallowReduce(const ReductionContext& reductionContext) override;

 private:
  /* Simplification */
  LayoutShape leftLayoutShape() const override {
    return childAtIndex(0)->leftLayoutShape();
  };
  LayoutShape rightLayoutShape() const override {
    return childAtIndex(1)->rightLayoutShape();
  }
};

class Subtraction final
    : public ExpressionTwoChildren<Subtraction, SubtractionNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder, ExpressionBuilder::Builder;
  static Subtraction Builder() {
    return PoolHandle::FixedArityBuilder<Subtraction, SubtractionNode>();
  }
  // OExpression
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
