#ifndef POINCARE_VECTOR_NORM_H
#define POINCARE_VECTOR_NORM_H

#include "old_expression.h"

namespace Poincare {

class VectorNormNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "norm";

  // PoolObject
  size_t size() const override { return sizeof(VectorNormNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "VectorNorm";
  }
#endif

  // Properties
  OMG::Troolean isPositive(Context* context) const override {
    return OMG::Troolean::True;
  }
  Type otype() const override { return Type::VectorNorm; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
};

class VectorNorm final : public ExpressionOneChild<VectorNorm, VectorNormNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
