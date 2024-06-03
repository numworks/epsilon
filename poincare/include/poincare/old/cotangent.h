#ifndef POINCARE_COTANGENT_H
#define POINCARE_COTANGENT_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class CotangentNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "cot";

  // PoolObject
  size_t size() const override { return sizeof(CotangentNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Cotangent";
  }
#endif

  // Properties
  Type otype() const override { return Type::Cotangent; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplication
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
};

class Cotangent final : public ExpressionOneChild<Cotangent, CotangentNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
};

}  // namespace Poincare

#endif
