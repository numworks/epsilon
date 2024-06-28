#ifndef POINCARE_DIVISION_REMAINDER_H
#define POINCARE_DIVISION_REMAINDER_H

#include "approximation_helper.h"
#include "integer.h"
#include "old_expression.h"

namespace Poincare {

class DivisionRemainderNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "rem";

  // PoolObject
  size_t size() const override { return sizeof(DivisionRemainderNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "DivisionRemainder";
  }
#endif

  // ExpressionNode
  OMG::Troolean isPositive(Context* context) const override {
    return OMG::Troolean::True;
  }
  Type otype() const override { return Type::DivisionRemainder; }

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

class DivisionRemainder final
    : public ExpressionTwoChildren<DivisionRemainder, DivisionRemainderNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  // OExpression
  OExpression shallowReduce(ReductionContext reductionContext);
  static OExpression Reduce(const Integer& a, const Integer& b);
};

}  // namespace Poincare

#endif
