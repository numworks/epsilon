#ifndef POINCARE_ARC_SECANT_H
#define POINCARE_ARC_SECANT_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class ArcSecantNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "arcsec";

  // PoolObject
  size_t size() const override { return sizeof(ArcSecantNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ArcSecant";
  }
#endif

  // Properties
  Type otype() const override { return Type::ArcSecant; }

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

class ArcSecant final : public ExpressionOneChild<ArcSecant, ArcSecantNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
};

}  // namespace Poincare

#endif
