#ifndef POINCARE_COMPLEX_CARTESIAN_H
#define POINCARE_COMPLEX_CARTESIAN_H

#include <poincare/expression.h>
#include <poincare/multiplication.h>

namespace Poincare {

class ComplexCartesianNode : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(ComplexCartesianNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "ComplexCartesian";
  }
#endif

  // Properties
  Type type() const override { return Type::ComplexCartesian; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { assert(false); return Layout(); }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  // Simplification
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  Expression shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) override;

private:
  template<typename T> Complex<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class ComplexCartesian final : public Expression {
public:
  ComplexCartesian() : Expression() {}
  ComplexCartesian(const ComplexCartesianNode * node) : Expression(node) {}
  static ComplexCartesian Builder() { ComplexCartesianNode * node = TreePool::sharedPool()->createTreeNode<ComplexCartesianNode>(); return ComplexCartesian(node); }
  static ComplexCartesian Builder(Expression child0, Expression child1) { return ComplexCartesian(child0, child1); }

  // Getters
  Expression real() { return childAtIndex(0); }
  Expression imag() { return childAtIndex(1); }

  // Simplification
  Expression shallowReduce();
  Expression shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);

  // Common operations (done in-place)
  Expression squareNorm(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression norm(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  Expression argument(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  ComplexCartesian inverse(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  ComplexCartesian squareRoot(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  ComplexCartesian powerInteger(int n, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  ComplexCartesian multiply(ComplexCartesian & other, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  ComplexCartesian power(ComplexCartesian & other, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
private:
  static constexpr int k_maxNumberOfNodesBeforeInterrupting = 50;
  ComplexCartesian(Expression child0, Expression child1) : Expression(TreePool::sharedPool()->createTreeNode<ComplexCartesianNode>()) {
    replaceChildAtIndexInPlace(0, child0);
    replaceChildAtIndexInPlace(1, child1);
  }
  void factorAndArgumentOfFunction(Expression e, ExpressionNode::Type searchedType, Expression * factor, Expression * argument, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  ComplexCartesian interruptComputationIfManyNodes();
  static Multiplication squareRootHelper(Expression e, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  static Expression powerHelper(Expression norm, Expression trigo, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
};


}

#endif
