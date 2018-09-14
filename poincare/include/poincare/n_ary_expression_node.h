#ifndef POINCARE_N_ARY_EXPRESSION_NODE_H
#define POINCARE_N_ARY_EXPRESSION_NODE_H

#include <poincare/expression.h>

namespace Poincare {

class NAryExpressionNode : public ExpressionNode { // TODO: VariableArityExpressionNode?
public:
  void setChildrenInPlace(Expression other) override { assert(false); }

  //Tree
  int numberOfChildren() const override { return m_numberOfChildren; }
  void incrementNumberOfChildren(int increment = 1) override { m_numberOfChildren+= increment; }
  void decrementNumberOfChildren(int decrement = 1) override {
    assert(m_numberOfChildren >= decrement);
    m_numberOfChildren-= decrement;
  }
  void eraseNumberOfChildren() override { m_numberOfChildren = 0; }

  // Comparison
  typedef int (*ExpressionOrder)(const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted);

  // Commutative properties
  void sortChildrenInPlace(ExpressionOrder order, bool canBeInterrupted);
  Expression squashUnaryHierarchyInPlace();

protected:
  /* With a pool of size < 120k and TreeNode of size 20, a node can't have more
   * than 6144 children which fit in uint16_t. */
  uint16_t m_numberOfChildren;
private:
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;
  int simplificationOrderGreaterType(const ExpressionNode * e, bool canBeInterrupted) const override;
};

class NAryExpression : public Expression {
public:
  NAryExpression(const NAryExpressionNode * n) : Expression(n) {}
  using TreeHandle::addChildAtIndexInPlace;
  using TreeHandle::removeChildrenInPlace;
  using TreeHandle::removeChildAtIndexInPlace;
  using TreeHandle::removeChildInPlace;
  typedef int (*ExpressionOrder)(const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted);
  void sortChildrenInPlace(ExpressionOrder order, bool canBeInterrupted) {
    node()->sortChildrenInPlace(order, canBeInterrupted);
  }
  Expression squashUnaryHierarchyInPlace() {
    return node()->squashUnaryHierarchyInPlace();
  }
protected:
  NAryExpressionNode * node() const { return static_cast<NAryExpressionNode *>(Expression::node()); }
};

}

#endif
