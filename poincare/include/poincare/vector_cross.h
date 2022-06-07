#ifndef POINCARE_VECTOR_CROSS_H
#define POINCARE_VECTOR_CROSS_H

#include <poincare/expression.h>

namespace Poincare {

class VectorCrossNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(VectorCrossNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "VectorCross";
  }
#endif

  // Properties
  Type type() const override { return Type::VectorCross; }
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

class VectorCross final : public Expression {
public:
  VectorCross(const VectorCrossNode * n) : Expression(n) {}
  static VectorCross Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<VectorCross, VectorCrossNode>({child0, child1}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("cross", 2, Initializer<VectorCrossNode>, sizeof(VectorCrossNode));

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
