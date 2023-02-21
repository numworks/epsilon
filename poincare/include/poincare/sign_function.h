#ifndef POINCARE_SIGN_FUNCTION_H
#define POINCARE_SIGN_FUNCTION_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class SignFunctionNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "sign";

  // TreeNode
  size_t size() const override { return sizeof(SignFunctionNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "SignFunction";
  }
#endif

  // Properties
  Type type() const override { return Type::SignFunction; }
  TrinaryBoolean isPositive(Context* context) const override {
    return childAtIndex(0)->isPositive(context);
  }
  TrinaryBoolean isNull(Context* context) const override {
    return childAtIndex(0)->isNull(context);
  }

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
                                     Preferences::AngleUnit angleUnit);
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
  // Derivation
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue) override;
};

class SignFunction final
    : public ExpressionOneChild<SignFunction, SignFunctionNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  Expression shallowReduce(ReductionContext reductionContext);
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue);
};

}  // namespace Poincare

#endif
