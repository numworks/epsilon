#ifndef POINCARE_MATRIX_TRACE_H
#define POINCARE_MATRIX_TRACE_H

#include <poincare/expression.h>

namespace Poincare {

class MatrixTraceNode /*final*/ : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(MatrixTraceNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "MatrixTrace";
  }
#endif

  // Properties
  Type type() const override { return Type::MatrixTrace; }
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

class MatrixTrace final : public Expression {
public:
  MatrixTrace(const MatrixTraceNode * n) : Expression(n) {}
  static MatrixTrace Builder(Expression child) { return TreeHandle::FixedArityBuilder<MatrixTrace, MatrixTraceNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("trace", 1, &UntypedBuilderOneChild<MatrixTrace>);

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
