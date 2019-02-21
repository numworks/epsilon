#ifndef POINCARE_MATRIX_DIMENSION_H
#define POINCARE_MATRIX_DIMENSION_H

#include <poincare/expression.h>

namespace Poincare {

class MatrixDimensionNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(MatrixDimensionNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "MatrixDimension";
  }
#endif

  // Properties
  Type type() const override { return Type::MatrixDimension; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
 template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class MatrixDimension final : public Expression {
public:
  MatrixDimension(const MatrixDimensionNode * n) : Expression(n) {}
  static MatrixDimension Builder(Expression child);
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("dim", 1, &UntypedBuilder);

  Expression shallowReduce();
};

}

#endif
