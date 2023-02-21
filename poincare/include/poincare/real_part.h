#ifndef POINCARE_REEL_PART_H
#define POINCARE_REEL_PART_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class RealPartNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "re";

  // TreeNode
  size_t size() const override { return sizeof(RealPartNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "RealPart";
  }
#endif

  // Properties
  TrinaryBoolean isPositive(Context* context) const override {
    return childAtIndex(0)->isPositive(context);
  }
  TrinaryBoolean isNull(Context* context) const override {
    return childAtIndex(0)->isNull(context) == TrinaryBoolean::True
               ? TrinaryBoolean::True
               : TrinaryBoolean::Unknown;
  }
  Type type() const override { return Type::RealPart; }

 private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
  // Evaluation
  template <typename T>
  static Complex<T> computeOnComplex(const std::complex<T> c,
                                     Preferences::ComplexFormat complexFormat,
                                     Preferences::AngleUnit angleUnit) {
    return Complex<T>::Builder(std::real(c));
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
  Expression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
