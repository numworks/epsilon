#ifndef POINCARE_COMPLEX_ARGUMENT_H
#define POINCARE_COMPLEX_ARGUMENT_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class ComplexArgumentNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "arg";

  // PoolObject
  size_t size() const override { return sizeof(ComplexArgumentNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ComplexArgument";
  }
#endif

  // Properties
  Type otype() const override { return Type::ComplexArgument; }

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

class ComplexArgument final
    : public ExpressionOneChild<ComplexArgument, ComplexArgumentNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
