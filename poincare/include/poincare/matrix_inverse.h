#ifndef POINCARE_MATRIX_INVERSE_H
#define POINCARE_MATRIX_INVERSE_H

#include <poincare/expression.h>

namespace Poincare {

class MatrixInverseNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(MatrixInverseNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "MatrixInverse";
  }
#endif

  // Properties
  Type type() const override { return Type::MatrixInverse; }
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

class MatrixInverse final : public Expression {
public:
  MatrixInverse(const MatrixInverseNode * n) : Expression(n) {}
  static MatrixInverse Builder(Expression child) { return TreeHandle::FixedArityBuilder<MatrixInverse, MatrixInverseNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("inverse", 1, Initializer<MatrixInverseNode>, sizeof(MatrixInverseNode));

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
