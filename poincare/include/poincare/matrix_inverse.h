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
  virtual void logNodeName(std::ostream & stream) const override {
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
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class MatrixInverse final : public Expression {
public:
  MatrixInverse(const MatrixInverseNode * n) : Expression(n) {}
  static MatrixInverse Builder(Expression child) { return TreeHandle::FixedArityBuilder<MatrixInverse, MatrixInverseNode>(&child, 1); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("inverse", 1, &UntypedBuilderOneChild<MatrixInverse>);

  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
};

}

#endif
