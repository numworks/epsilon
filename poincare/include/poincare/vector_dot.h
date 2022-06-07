#ifndef POINCARE_VECTOR_DOT_H
#define POINCARE_VECTOR_DOT_H

#include <poincare/expression.h>

namespace Poincare {

class VectorDotNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(VectorDotNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "VectorDot";
  }
#endif

  // Properties
  Type type() const override { return Type::VectorDot; }
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

class VectorDot final : public Expression {
public:
  VectorDot(const VectorDotNode * n) : Expression(n) {}
  static VectorDot Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<VectorDot, VectorDotNode>({child0, child1}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("dot", 2, Initializer<VectorDotNode>, sizeof(VectorDotNode));

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
