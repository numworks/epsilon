#ifndef POINCARE_MULTIPLICATION_IMPLICITE_H
#define POINCARE_MULTIPLICATION_IMPLICITE_H

#include <poincare/multiplication.h>

namespace Poincare {

class MultiplicationImplicite;

class MultiplicationImpliciteNode /*final*/ : public MultiplicationNode {
public:
  using MultiplicationNode::MultiplicationNode;

  // Tree
  size_t size() const override { return sizeof(MultiplicationImpliciteNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Multiplication Implicite";
  }
#endif

  // Properties
  Type type() const override { return Type::MultiplicationImplicite; }

private:
  // Layout
  bool childNeedsParenthesis(const TreeNode * child) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Serialize
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
};

class MultiplicationImplicite : public Multiplication {
public:
  MultiplicationImplicite(const MultiplicationImpliciteNode * n) : Multiplication(n) {}
  static MultiplicationImplicite Builder(Expression e1, Expression e2) { return MultiplicationImplicite::Builder(ArrayBuilder<Expression>(e1, e2).array(), 2); }
  static MultiplicationImplicite Builder(Expression * children, size_t numberOfChildren) { return TreeHandle::NAryBuilder<MultiplicationImplicite, MultiplicationImpliciteNode>(children, numberOfChildren); }
  // Simplification
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
