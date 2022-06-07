#ifndef POINCARE_MATRIX_TRANSPOSE_H
#define POINCARE_MATRIX_TRANSPOSE_H

#include <poincare/expression.h>

namespace Poincare {

class MatrixTransposeNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(MatrixTransposeNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "MatrixTranspose";
  }
#endif

  // Properties
  Type type() const override { return Type::MatrixTranspose; }
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

class MatrixTranspose final : public Expression {
public:
  MatrixTranspose(const MatrixTransposeNode * n) : Expression(n) {}
  static MatrixTranspose Builder(Expression child) { return TreeHandle::FixedArityBuilder<MatrixTranspose, MatrixTransposeNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("transpose", 1, Initializer<MatrixTransposeNode>, sizeof(MatrixTransposeNode));

  Expression shallowReduce(Context * context);
};

}

#endif
