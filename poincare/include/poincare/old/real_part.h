#ifndef POINCARE_REEL_PART_H
#define POINCARE_REEL_PART_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class RealPartNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "re";

  // PoolObject
  size_t size() const override { return sizeof(RealPartNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "RealPart";
  }
#endif

  // Properties
  OMG::Troolean isPositive(Context* context) const override {
    return childAtIndex(0)->isPositive(context);
  }
  OMG::Troolean isNull(Context* context) const override {
    return childAtIndex(0)->isNull(context) == OMG::Troolean::True
               ? OMG::Troolean::True
               : OMG::Troolean::Unknown;
  }
  Type otype() const override { return Type::RealPart; }

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
  // Evaluation
  template <typename T>
  static std::complex<T> computeOnComplex(
      const std::complex<T> c, Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit) {
    return std::real(c);
  }
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

class RealPart final : public ExpressionOneChild<RealPart, RealPartNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
