#ifndef POINCARE_VECTOR_DOT_H
#define POINCARE_VECTOR_DOT_H

#include "old_expression.h"

namespace Poincare {

class VectorDotNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "dot";

  // PoolObject
  size_t size() const override { return sizeof(VectorDotNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "VectorDot";
  }
#endif

  // Properties
  Type otype() const override { return Type::VectorDot; }

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

class VectorDot final : public ExpressionTwoChildren<VectorDot, VectorDotNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
