#ifndef POINCARE_N_ARY_EXPRESSION_H
#define POINCARE_N_ARY_EXPRESSION_H

#include <poincare/expression.h>

// NAryExpressions are additions and multiplications

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

  // Properties
  bool childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const override;

  // Comparison
  typedef int (*ExpressionOrder)(const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted);

  // Commutative properties
  void sortChildrenInPlace(ExpressionOrder order, Context * context, bool canSwapMatrices, bool canBeInterrupted);
  Expression squashUnaryHierarchyInPlace();

protected:
  LayoutShape leftLayoutShape() const override { return childAtIndex(0)->leftLayoutShape(); };
  LayoutShape rightLayoutShape() const override { return childAtIndex(numberOfChildren()-1)->rightLayoutShape(); }

  /* With a pool of size < 120k and TreeNode of size 20, a node can't have more
   * than 6144 children which fit in uint16_t. */
  uint16_t m_numberOfChildren;
private:
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const override;
  int simplificationOrderGreaterType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const override;
};

class NAryExpression : public Expression {
public:
  NAryExpression(const NAryExpressionNode * n) : Expression(n) {}
  using TreeHandle::addChildAtIndexInPlace;
  using TreeHandle::removeChildrenInPlace;
  using TreeHandle::removeChildAtIndexInPlace;
  using TreeHandle::removeChildInPlace;
  Expression squashUnaryHierarchyInPlace() {
    return node()->squashUnaryHierarchyInPlace();
  }
  /* allChildrenAreReal returns:
   * - 1 if all children are real
   * - 0 if all non real children are ComplexCartesian
   * - -1 if some chidren are non-real and non ComplexCartesian */
  int allChildrenAreReal(Context * context) const;
protected:
  void sortChildrenInPlace(NAryExpressionNode::ExpressionOrder order, Context * context, bool canSwapMatrices, bool canBeInterrupted) {
    node()->sortChildrenInPlace(order, context, canSwapMatrices, canBeInterrupted);
  }
  NAryExpressionNode * node() const { return static_cast<NAryExpressionNode *>(Expression::node()); }
};

}

#endif
