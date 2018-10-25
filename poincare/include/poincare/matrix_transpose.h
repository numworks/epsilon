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
  virtual void logNodeName(std::ostream & stream) const override {
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
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class MatrixTranspose final : public Expression {
public:
  MatrixTranspose(const MatrixTransposeNode * n) : Expression(n) {}
  static MatrixTranspose Builder(Expression child) { return MatrixTranspose(child); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("transpose", 1, &UntypedBuilder);

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
private:
  explicit MatrixTranspose(Expression child) : Expression(TreePool::sharedPool()->createTreeNode<MatrixTransposeNode>()) {
    replaceChildAtIndexInPlace(0, child);
  }
};

}

#endif
