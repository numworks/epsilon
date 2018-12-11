#ifndef POINCARE_COMPLEX_CARTESIAN_H
#define POINCARE_COMPLEX_CARTESIAN_H

#include <poincare/expression.h>
#include <poincare/complex_helper.h>

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
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { assert(false); return Evaluation<float>(); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { assert(false); return Evaluation<double>(); }
};

class ComplexCartesian final : public Expression {
public:
  ComplexCartesian() : Expression() {}
  static ComplexCartesian Builder(Expression child0, Expression child1) { return ComplexCartesian(child0, child1); }
  Expression real() { return childAtIndex(0); }
  Expression imag() { return childAtIndex(1); }
private:
  ComplexCartesian(Expression child0, Expression child1) : Expression(TreePool::sharedPool()->createTreeNode<ComplexCartesianNode>()) {
    replaceChildAtIndexInPlace(0, child0);
    replaceChildAtIndexInPlace(1, child1);
  }
};


}

#endif
