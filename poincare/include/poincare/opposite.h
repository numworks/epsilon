#ifndef POINCARE_OPPOSITE_H
#define POINCARE_OPPOSITE_H

#include <poincare/expression_reference.h>
#include <poincare/approximation_engine.h>

namespace Poincare {

class OppositeNode : public ExpressionNode {
public:
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
  EvaluationReference<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationEngine::map<float>(this, context, angleUnit, compute<float>);
  }
  EvaluationReference<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationEngine::map<double>(this, context, angleUnit, compute<double>);
  }

  // Layout
  bool needsParenthesisWithParent(SerializableNode * parentNode) const override;
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;

  // Simplification
  ExpressionReference shallowReduce(Context& context, Preferences::AngleUnit angleUnit) override;

private:
  template<typename T> static ComplexReference<T> compute(const std::complex<T> c, Preferences::AngleUnit angleUnit) { return ComplexReference<T>(-c); }
};

class OppositeReference : public ExpressionReference {
public:
  OppositeReference(ExpressionReference operand) : ExpressionReference(TreePool::sharedPool()->createTreeNode<OppositeNode>(), true) {
    replaceChildAtIndexInPlace(0, operand);
  }
};

}

#endif
