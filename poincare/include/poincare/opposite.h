#ifndef POINCARE_OPPOSITE_H
#define POINCARE_OPPOSITE_H

#include <poincare/expression.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class Opposite;

class OppositeNode : public ExpressionNode {
public:
  template<typename T> static Complex<T> compute(const std::complex<T> c, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Degree) { return Complex<T>(-c); }

  // Allocation Failure
  static OppositeNode * FailedAllocationStaticNode();
  OppositeNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  // TreeNode
  size_t size() const override { return sizeof(OppositeNode); }
#if TREE_LOG
  const char * description() const override { return "Opposite";  }
#endif
  int numberOfChildren() const override { return 0; }

  // Properties
  Type type() const override { return Type::Opposite; }
  int polynomialDegree(char symbolName) const override;
  Sign sign() const override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit, compute<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, compute<double>);
  }

  // Layout
  bool needsParenthesesWithParent(const SerializationHelperInterface * parent) const override;
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;

  // Simplification
  virtual Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const override;
};

class Opposite : public Expression {
public:
  Opposite(const OppositeNode * n) : Expression(n) {}
  Opposite(Expression operand) : Expression(TreePool::sharedPool()->createTreeNode<OppositeNode>()) {
    replaceChildAtIndexInPlace(0, operand);
  }

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
