#ifndef POINCARE_VECTOR_NORM_H
#define POINCARE_VECTOR_NORM_H

#include <poincare/expression.h>

namespace Poincare {

class VectorNormNode final : public ExpressionNode {
public:
  static constexpr char functionName[] = "norm";

  // TreeNode
  size_t size() const override { return sizeof(VectorNormNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "VectorNorm";
  }
#endif

  // Properties
  Sign sign(Context * context) const override { return Sign::Positive; }
  Type type() const override { return Type::VectorNorm; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class VectorNorm final : public HandleOneChild<VectorNorm, VectorNormNode> {
public:
  using Handle::Handle, Handle::Builder, Handle::s_functionHelper;
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
