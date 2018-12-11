#ifndef POINCARE_COMPLEX_POLAR_H
#define POINCARE_COMPLEX_POLAR_H

#include <poincare/expression.h>
#include <poincare/complex_helper.h>

namespace Poincare {

class ComplexPolarNode : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(ComplexPolarNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "ComplexPolar";
  }
#endif

  // Properties
  Type type() const override { return Type::ComplexPolar; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { assert(false); return Layout(); }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { assert(false); return Evaluation<float>(); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { assert(false); return Evaluation<double>(); }
};

class ComplexPolar final : public Expression {
public:
  ComplexPolar() : Expression() {}
  static ComplexPolar Builder(Expression child0, Expression child1) { return ComplexPolar(child0, child1); }
  Expression norm() { return childAtIndex(0); }
  Expression arg() { return childAtIndex(1); }
private:
  ComplexPolar(Expression child0, Expression child1) : Expression(TreePool::sharedPool()->createTreeNode<ComplexPolarNode>()) {
    replaceChildAtIndexInPlace(0, child0);
    replaceChildAtIndexInPlace(1, child1);
  }
};


}

#endif
