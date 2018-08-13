#ifndef POINCARE_N_ARY_EXPRESSION_NODE_H
#define POINCARE_N_ARY_EXPRESSION_NODE_H

#include <poincare/expression.h>

namespace Poincare {

class NAryExpressionNode : public ExpressionNode { // TODO: VariableArityExpressionNode?
public:
  //Tree
  int numberOfChildren() const override { return m_numberOfChildren; }
  void incrementNumberOfChildren(int increment = 1) override { /*TODO override for alloc fail ?*/ m_numberOfChildren+= increment; }
  void decrementNumberOfChildren(int decrement = 1) override {
    /*TODO override for alloc fail ?*/
    assert(m_numberOfChildren >= decrement);
    m_numberOfChildren-= decrement;
  }
  void eraseNumberOfChildren() override { m_numberOfChildren = 0; }

  // Comparison
  typedef int (*ExpressionOrder)(const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted);

  // Commutative properties
  void sortChildrenInPlace(ExpressionOrder order, bool canBeInterrupted);
  Expression squashUnaryHierarchy();

protected:
  int m_numberOfChildren;
private:
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;
  int simplificationOrderGreaterType(const ExpressionNode * e, bool canBeInterrupted) const override;
};

class NAryExpression : public Expression {
public:
  NAryExpression(const NAryExpressionNode * n) : Expression(n) {}
  using Expression::addChildAtIndexInPlace;
  using Expression::removeChildrenInPlace;
  using Expression::removeChildrenAndDestroyInPlace;
  using Expression::removeChildAtIndexInPlace;
  using Expression::removeChildInPlace;
  typedef int (*ExpressionOrder)(const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted);
  void sortChildrenInPlace(ExpressionOrder order, bool canBeInterrupted) {
    node()->sortChildrenInPlace(order, canBeInterrupted);
  }
  Expression squashUnaryHierarchy() {
    return node()->squashUnaryHierarchy();
  }
protected:
  NAryExpressionNode * node() const { return static_cast<NAryExpressionNode *>(Expression::node()); }
};

}

#endif
