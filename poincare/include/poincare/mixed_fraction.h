#ifndef POINCARE_MIXED_FRACTION_H
#define POINCARE_MIXED_FRACTION_H

#include <poincare/expression.h>
#include <poincare/division.h>

namespace Poincare {

class MixedFractionNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(MixedFractionNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "MixedFraction";
  }
#endif

  // Properties
  Type type() const override { return Type::MixedFraction; }
  TrinaryBoolean isPositive(Context * context) const override { return childAtIndex(0)->isPositive(context); }
private:
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::Integer; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::Fraction; }
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override {
    return templateApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override {
    return templateApproximate<double>(approximationContext);
  }
  template <typename T> Evaluation<T> templateApproximate(const ApproximationContext& approximationContext) const;
};

class MixedFraction final : public Expression {
public:
  MixedFraction(const MixedFractionNode * n) : Expression(n) {}
  static Expression Builder(Expression integerPart, Expression numerator, Expression denominator) { return Builder(integerPart, Division::Builder(numerator, denominator)); }
  static Expression Builder(Expression integerPart, Division fractionPart) { return TreeHandle::FixedArityBuilder<MixedFraction, MixedFractionNode>({integerPart, fractionPart}); }

  Expression shallowReduce(ReductionContext context);
};

}

#endif
