#ifndef POINCARE_SQUARE_ROOT_H
#define POINCARE_SQUARE_ROOT_H

#include "approximation_helper.h"
#include "multiplication.h"
#include "old_expression.h"

namespace Poincare {

class SquareRootNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName =
      AliasesLists::k_squareRootAliases;

  // ExpressionNode
  OMG::Troolean isPositive(Context* context) const override {
    return childAtIndex(0)->isPositive(context) == OMG::Troolean::True
               ? OMG::Troolean::True
               : OMG::Troolean::Unknown;
  }
  OMG::Troolean isNull(Context* context) const override {
    return childAtIndex(0)->isNull(context);
  }
  Type otype() const override { return Type::SquareRoot; }

  // PoolObject
  size_t size() const override { return sizeof(SquareRootNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "SquareRoot";
  }
#endif

  template <typename T>
  static std::complex<T> computeOnComplex(
      const std::complex<T> c, Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit);

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::Root; };
  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext,
                                                   computeOnComplex<float>);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext,
                                                    computeOnComplex<double>);
  }
};

class SquareRoot final : public ExpressionOneChild<SquareRoot, SquareRootNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  /* Reduce an expression of the form √(a√b + c√d) */
  static OExpression ReduceNestedRadicals(
      OExpression a, OExpression b, OExpression c, OExpression d,
      const ReductionContext& reductionContext);
  static bool SplitRadical(OExpression term, OExpression* factor,
                           OExpression* underRoot);
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
