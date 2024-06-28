#ifndef POINCARE_OPPOSITE_H
#define POINCARE_OPPOSITE_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class Opposite;

class OppositeNode final : public ExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(OppositeNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Opposite";
  }
#endif

  // Properties
  OMG::Troolean isNull(Context* context) const override {
    return childAtIndex(0)->isNull(context);
  }
  Type otype() const override { return Type::Opposite; }
  OMG::Troolean isPositive(Context* context) const override {
    return TrooleanNot(childAtIndex(0)->isPositive(context));
  }
  bool childAtIndexNeedsUserParentheses(const OExpression& child,
                                        int childIndex) const override;

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode =
                       Preferences::PrintFloatMode::Decimal,
                   int numberOfSignificantDigits = 0) const override;

  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    // leftLayoutShape of Opposite is only called from Conjugate
    assert(parent() && parent()->otype() == Type::Conjugate);
    return LayoutShape::OneLetter;
  };
  LayoutShape rightLayoutShape() const override {
    return childAtIndex(0)->rightLayoutShape();
  }
};

class Opposite final : public ExpressionOneChild<Opposite, OppositeNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder, ExpressionBuilder::Builder;
  static Opposite Builder() {
    return PoolHandle::FixedArityBuilder<Opposite, OppositeNode>();
  }
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
