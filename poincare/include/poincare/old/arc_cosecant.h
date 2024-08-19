#ifndef POINCARE_ARC_COSCANT_H
#define POINCARE_ARC_COSCANT_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class ArcCosecantNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "arccsc";

  // PoolObject
  size_t size() const override { return sizeof(ArcCosecantNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ArcCosecant";
  }
#endif

  // Properties
  Type otype() const override { return Type::ArcCosecant; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplication
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
};

class ArcCosecant final
    : public ExpressionOneChild<ArcCosecant, ArcCosecantNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
};

}  // namespace Poincare

#endif
