#ifndef POINCARE_COSCANT_H
#define POINCARE_COSCANT_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class CosecantNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "csc";

  // PoolObject
  size_t size() const override { return sizeof(CosecantNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Cosecant";
  }
#endif

  // Properties
  Type otype() const override { return Type::Cosecant; }

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

class Cosecant final : public ExpressionOneChild<Cosecant, CosecantNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
};

}  // namespace Poincare

#endif
