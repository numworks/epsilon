#ifndef POINCARE_DIVISION_H
#define POINCARE_DIVISION_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class Division;

class DivisionNode final : public ExpressionNode {
  friend class LogarithmNode;

 public:
  // PoolObject
  size_t size() const override { return sizeof(DivisionNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Division";
  }
#endif

  // Properties
  OMG::Troolean isPositive(Context* context) const override;
  OMG::Troolean isNull(Context* context) const override {
    // NonNull Status can't be returned because denominator could be infinite.
    return childAtIndex(0)->isNull(context) == OMG::Troolean::True
               ? OMG::Troolean::True
               : OMG::Troolean::Unknown;
  }
  Type otype() const override { return Type::Division; }
  OExpression removeUnit(OExpression* unit) override {
    assert(false);
    return ExpressionNode::removeUnit(unit);
  }

  // Layout
  bool childNeedsSystemParenthesesAtSerialization(
      const PoolObject* child) const override;
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::Fraction;
  };
};

class Division final : public ExpressionTwoChildren<Division, DivisionNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
