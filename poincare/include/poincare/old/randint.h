#ifndef POINCARE_RANDINT_H
#define POINCARE_RANDINT_H

#include "expression_node_with_up_to_two_children.h"
#include "old_expression.h"
#include "random.h"
#include "rational.h"

namespace Poincare {

class RandintNode final : public ExpressionNodeWithOneOrTwoChildren {
  friend class Randint;

 public:
  constexpr static AliasesList k_functionName = "randint";

  // PoolObject
  size_t size() const override { return sizeof(RandintNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Randint"; }
#endif

  // Properties
  Type otype() const override { return Type::Randint; }

 private:
  constexpr static int k_defaultMinBound = 1;
  OExpression createExpressionWithTwoChildren() const;
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templateApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templateApproximate<double>(approximationContext);
  }
  template <typename T>
  Evaluation<T> templateApproximate(
      const ApproximationContext& approximationContext,
      bool* inputIsUndefined = nullptr) const;

  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
};

class Randint final : public ExpressionUpToTwoChildren<Randint, RandintNode> {
  friend class RandintNode;

 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
