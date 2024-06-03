#ifndef POINCARE_SECANT_H
#define POINCARE_SECANT_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class SecantNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "sec";

  // PoolObject
  size_t size() const override { return sizeof(SecantNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Secant"; }
#endif

  // Properties
  Type otype() const override { return Type::Secant; }

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

class Secant final : public ExpressionOneChild<Secant, SecantNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
};

}  // namespace Poincare

#endif
