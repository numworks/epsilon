#ifndef POINCARE_VECTOR_CROSS_H
#define POINCARE_VECTOR_CROSS_H

#include "old_expression.h"

namespace Poincare {

class VectorCrossNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "cross";

  // PoolObject
  size_t size() const override { return sizeof(VectorCrossNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "VectorCross";
  }
#endif

  // Properties
  Type otype() const override { return Type::VectorCross; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
};

class VectorCross final
    : public ExpressionTwoChildren<VectorCross, VectorCrossNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
