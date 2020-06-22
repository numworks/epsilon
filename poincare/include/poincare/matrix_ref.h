#ifndef POINCARE_MATRIX_REF_H
#define POINCARE_MATRIX_REF_H

#include <poincare/expression.h>

namespace Poincare {

class MatrixRefNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(MatrixRefNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "MatrixRef";
  }
#endif

  // Properties
  Type type() const override { return Type::MatrixRef; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class MatrixRef final : public Expression {
public:
  MatrixRef(const MatrixRefNode * n) : Expression(n) {}
  static MatrixRef Builder(Expression child) { return TreeHandle::FixedArityBuilder<MatrixRef, MatrixRefNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("ref", 1, &UntypedBuilderOneChild<MatrixRef>);

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
