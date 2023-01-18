#ifndef POINCARE_VECTOR_NORM_H
#define POINCARE_VECTOR_NORM_H

#include <poincare/expression.h>

namespace Poincare {

class VectorNormNode final : public ExpressionNode {
public:
  constexpr static AliasesList k_functionName = "norm";

  // TreeNode
  size_t size() const override { return sizeof(VectorNormNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "VectorNorm";
  }
#endif

  // Properties
  TrinaryBoolean isPositive(Context * context) const override { return TrinaryBoolean::True; }
  Type type() const override { return Type::VectorNorm; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class VectorNorm final : public ExpressionOneChild<VectorNorm, VectorNormNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(ReductionContext reductionContext);
};

}

#endif
