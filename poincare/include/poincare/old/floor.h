#ifndef POINCARE_FLOOR_H
#define POINCARE_FLOOR_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class FloorNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "floor";

  // PoolObject
  size_t size() const override { return sizeof(FloorNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Floor"; }
#endif

  // Properties
  Type otype() const override { return Type::Floor; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  };
};

class Floor final : public ExpressionOneChild<Floor, FloorNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
