#ifndef POINCARE_FACTORIAL_H
#define POINCARE_FACTORIAL_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class FactorialNode final : public ExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(FactorialNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Factorial";
  }
#endif

  // Properties
  OMG::Troolean isNull(Context* context) const override {
    return OMG::Troolean::False;
  }
  Type otype() const override { return Type::Factorial; }
  OMG::Troolean isPositive(Context* context) const override {
    return OMG::Troolean::True;
  }
  bool childAtIndexNeedsUserParentheses(const OExpression& child,
                                        int childIndex) const override;

 private:
  // Layout
  bool childNeedsSystemParenthesesAtSerialization(
      const PoolObject* child) const override;
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplication
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return childAtIndex(0)->leftLayoutShape();
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

#if 0
  int simplificationOrderGreaterType(const OExpression e) const override;
  int simplificationOrderSameType(const OExpression e) const override;
#endif
};

class Factorial final : public ExpressionOneChild<Factorial, FactorialNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);

 private:
  constexpr static int k_maxOperandValue = 100;
};

}  // namespace Poincare

#endif
