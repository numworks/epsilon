#ifndef POINCARE_ARC_COSINE_H
#define POINCARE_ARC_COSINE_H

#include <poincare/approximation_helper.h>
#include <poincare/arc_secant.h>
#include <poincare/expression.h>
#include <poincare/trigonometry.h>

namespace Poincare {

class ArcCosineNode final : public ExpressionNode {
  friend class ArcSecantNode;

 public:
  constexpr static AliasesList k_functionName = AliasesLists::k_acosAliases;

  // TreeNode
  size_t size() const override { return sizeof(ArcCosineNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ArcCosine";
  }
#endif

  // Properties
  TrinaryBoolean isPositive(Context* context) const override {
    return childAtIndex(0)->isPositive(context) == TrinaryBoolean::Unknown
               ? TrinaryBoolean::Unknown
               : TrinaryBoolean::True;
  }
  Type type() const override { return Type::ArcCosine; }

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

  // Derivation
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue) override;
  Expression unaryFunctionDifferential(
      const ReductionContext& reductionContext) override;

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
};

class ArcCosine final : public ExpressionOneChild<ArcCosine, ArcCosineNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue);
  Expression unaryFunctionDifferential(
      const ReductionContext& reductionContext);
};

}  // namespace Poincare

#endif
