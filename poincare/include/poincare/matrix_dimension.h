#ifndef POINCARE_MATRIX_DIMENSION_H
#define POINCARE_MATRIX_DIMENSION_H

#include <poincare/expression.h>

namespace Poincare {

class MatrixDimensionNode : public ExpressionNode {
public:
  // Allocation Failure
  static MatrixDimensionNode * FailedAllocationStaticNode();
  MatrixDimensionNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  // TreeNode
  size_t size() const override { return sizeof(MatrixDimensionNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "MatrixDimension";
  }
#endif

  // Properties
  Type type() const override { return Type::MatrixDimension; }
private:
  // Layout
  LayoutReference createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  const char * name() const { return "dim"; }
  // Simplification
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class MatrixDimension : public Expression {
public:
  MatrixDimension() : Expression(TreePool::sharedPool()->createTreeNode<MatrixDimensionNode>()) {}
  MatrixDimension(const MatrixDimensionNode * n) : Expression(n) {}
  MatrixDimension(Expression operand) : MatrixDimension() {
    replaceChildAtIndexInPlace(0, operand);
  }
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent);
};

}

#endif
