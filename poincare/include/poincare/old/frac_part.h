#ifndef POINCARE_FRAC_PART_H
#define POINCARE_FRAC_PART_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class FracPartNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "frac";

  // PoolObject
  size_t size() const override { return sizeof(FracPartNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "FracPart";
  }
#endif

  // Properties
  OMG::Troolean isPositive(Context* context) const override {
    return OMG::Troolean::True;
  }
  Type otype() const override { return Type::FracPart; }

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

class FracPart final : public ExpressionOneChild<FracPart, FracPartNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
