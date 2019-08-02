#ifndef POINCARE_MULTIPLICATION_IMPLICIT_H
#define POINCARE_MULTIPLICATION_IMPLICIT_H

#include <poincare/multiplication.h>

namespace Poincare {

class MultiplicationImplicit;

class MultiplicationImplicitNode /*final*/ : public MultiplicationNode {
public:
  using MultiplicationNode::MultiplicationNode;

  // Tree
  size_t size() const override { return sizeof(MultiplicationImplicitNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Multiplication Implicit";
  }
#endif

  // Properties
  Type type() const override { return Type::MultiplicationImplicit; }

private:
  // Layout
  bool childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Serialize
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
};

class MultiplicationImplicit : public Multiplication {
public:
  MultiplicationImplicit(const MultiplicationImplicitNode * n) : Multiplication(n) {}
  static MultiplicationImplicit Builder(Expression e1, Expression e2) { return MultiplicationImplicit::Builder(ArrayBuilder<Expression>(e1, e2).array(), 2); }
  static MultiplicationImplicit Builder(Expression * children, size_t numberOfChildren) { return TreeHandle::NAryBuilder<MultiplicationImplicit, MultiplicationImplicitNode>(children, numberOfChildren); }
  // Simplification
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
