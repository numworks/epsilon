#ifndef POINCARE_MATRIX_IDENTITY_H
#define POINCARE_MATRIX_IDENTITY_H

#include <poincare/expression.h>

namespace Poincare {

class MatrixIdentityNode final : public ExpressionNode {
public:
  static constexpr char functionName[] = "identity";

  // TreeNode
  size_t size() const override { return sizeof(MatrixIdentityNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "MatrixIdentity";
  }
#endif

  // Properties
  Type type() const override { return Type::MatrixIdentity; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class MatrixIdentity final : public HandleOneChild<MatrixIdentity, MatrixIdentityNode> {
public:
  using Handle::Handle, Handle::Builder, Handle::s_functionHelper;
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
