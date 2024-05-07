#ifndef POINCARE_MIXED_FRACTION_H
#define POINCARE_MIXED_FRACTION_H

#include "division.h"
#include "old_expression.h"

namespace Poincare {

class MixedFractionNode final : public ExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(MixedFractionNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "MixedFraction";
  }
#endif

  // Properties
  Type otype() const override { return Type::MixedFraction; }
  OMG::Troolean isPositive(Context* context) const override {
    return childAtIndex(0)->isPositive(context);
  }

 private:
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::Integer; };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::Fraction;
  }
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
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
      const ApproximationContext& approximationContext) const;
};

class MixedFraction final : public OExpression {
 public:
  MixedFraction(const MixedFractionNode* n) : OExpression(n) {}
  static OExpression Builder(OExpression integerPart, OExpression numerator,
                             OExpression denominator) {
    return Builder(integerPart, Division::Builder(numerator, denominator));
  }
  static OExpression Builder(OExpression integerPart, Division fractionPart) {
    return PoolHandle::FixedArityBuilder<MixedFraction, MixedFractionNode>(
        {integerPart, fractionPart});
  }

  OExpression shallowReduce(ReductionContext context);
};

}  // namespace Poincare

#endif
