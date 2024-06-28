#ifndef POINCARE_DETERMINANT_H
#define POINCARE_DETERMINANT_H

#include "old_expression.h"

namespace Poincare {

class DeterminantNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "det";

  // PoolObject
  size_t size() const override { return sizeof(DeterminantNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Determinant";
  }
#endif

  Type otype() const override { return Type::Determinant; }

 private:
  /* Layout */
  /* Serialization */
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  /* Simplification */
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
};

class Determinant final
    : public ExpressionOneChild<Determinant, DeterminantNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
