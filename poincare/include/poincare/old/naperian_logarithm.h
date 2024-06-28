#ifndef POINCARE_NAPERIAN_LOGARITHM_H
#define POINCARE_NAPERIAN_LOGARITHM_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class NaperianLogarithmNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "ln";

  // PoolObject
  size_t size() const override { return sizeof(NaperianLogarithmNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "NaperianLogarithm";
  }
#endif

  // Properties
  Type otype() const override { return Type::NaperianLogarithm; }

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

class NaperianLogarithm final
    : public ExpressionOneChild<NaperianLogarithm, NaperianLogarithmNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
