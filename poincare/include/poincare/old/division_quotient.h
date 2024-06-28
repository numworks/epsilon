#ifndef POINCARE_DIVISION_QUOTIENT_H
#define POINCARE_DIVISION_QUOTIENT_H

#include "integer.h"
#include "old_expression.h"

namespace Poincare {

class DivisionQuotientNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "quo";

  // PoolObject
  size_t size() const override { return sizeof(DivisionQuotientNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "DivisionQuotient";
  }
#endif

  // ExpressionNode
  OMG::Troolean isPositive(Context* context) const override;
  Type otype() const override { return Type::DivisionQuotient; }

  // Simplification
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
};

class DivisionQuotient final
    : public ExpressionTwoChildren<DivisionQuotient, DivisionQuotientNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  // OExpression
  OExpression shallowReduce(ReductionContext reductionContext);
  static OExpression Reduce(const Integer& a, const Integer& b);
};

}  // namespace Poincare

#endif
