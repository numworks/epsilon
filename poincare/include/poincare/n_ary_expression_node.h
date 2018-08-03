#ifndef POINCARE_NARY_EXPRESSION_NODE_H
#define POINCARE_NARY_EXPRESSION_NODE_H

#include <poincare/expression_reference.h>

namespace Poincare {

class NAryExpressionNode : public ExpressionNode { // TODO: VariableArityExpressionNode?
public:
  int numberOfChildren() const override { return m_numberOfChildren; }
  bool isChildRemovalTolerant() const override { return true; }
  void incrementNumberOfChildren(int increment = 1) { m_numberOfChildren+= increment; }
  void decrementNumberOfChildren(int decrement = 1) {
    assert(m_numberOfChildren >= decrement);
    m_numberOfChildren-= decrement;
  }
  void eraseNumberOfChildren() { m_numberOfChildren = 0; }

  // Comparison
  typedef int (*ExpressionOrder)(const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted);

  // Commutative properties
  void sortOperands(ExpressionOrder order, bool canBeInterrupted);
  ExpressionReference squashUnaryHierarchy();

protected:
  int m_numberOfChildren;
private:
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;
  int simplificationOrderGreaterType(const ExpressionNode * e, bool canBeInterrupted) const override;
};

}

#endif
