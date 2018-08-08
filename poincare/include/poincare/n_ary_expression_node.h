#ifndef POINCARE_NARY_EXPRESSION_NODE_H
#define POINCARE_NARY_EXPRESSION_NODE_H

#include <poincare/expression_reference.h>

namespace Poincare {

class NAryExpressionNode : public ExpressionNode { // TODO: VariableArityExpressionNode?
public:
  //Tree
  int numberOfChildren() const override { return m_numberOfChildren; }
  void incrementNumberOfChildren(int increment = 1) { m_numberOfChildren+= increment; }
  void decrementNumberOfChildren(int decrement = 1) {
    assert(m_numberOfChildren >= decrement);
    m_numberOfChildren-= decrement;
  }
  void eraseNumberOfChildren() { m_numberOfChildren = 0; }

  // Comparison
  typedef int (*ExpressionOrder)(const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted);

  // Commutative properties
  void sortOperandsInPlace(ExpressionOrder order, bool canBeInterrupted);
  Expression squashUnaryHierarchy();

protected:
  int m_numberOfChildren;
private:
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;
  int simplificationOrderGreaterType(const ExpressionNode * e, bool canBeInterrupted) const override;
};

class NAryExpressionRef : public Expression {
public:
  void addChildAtIndexInPlace(TreeReference t, int index, int currentNumberOfChildren) override {
    Expression::addChildAtIndexInPlace(t, index, currentNumberOfChildren);
  }
  // Remove puts a child at the end of the pool
  void removeChildAtIndexInPlace(int i) override {
    Expression::removeChildAtIndexInPlace(i);
  }
  void removeChildInPlace(TreeReference t, int childNumberOfChildren) override {
    Expression::removeChildInPlace(t, childNumberOfChildren);
  }
  void removeChildrenInPlace(int currentNumberOfChildren) {
    Expression::removeChildrenInPlace(currentNumberOfChildren);
  }
  void removeChildrenAndDestroyInPlace(int currentNumberOfChildren) {
    Expression::removeChildrenAndDestroyInPlace(currentNumberOfChildren);
  }
};

}

#endif
