#ifndef POINCARE_N_ARY_EXPRESSION_H
#define POINCARE_N_ARY_EXPRESSION_H

#include <poincare/expression.h>

namespace Poincare {

class NAryExpressionNode : public ExpressionNode { // TODO: VariableArityExpressionNode?
public:
#if ASSERTIONS
  void setChildrenInPlace(Expression other) override { assert(false); }
#endif

  //Tree
  int numberOfChildren() const override { return m_numberOfChildren; }
  void setNumberOfChildren(int numberOfChildren) override { m_numberOfChildren = numberOfChildren; }

  // Comparison
  typedef int (*ExpressionOrder)(const ExpressionNode * e1, const ExpressionNode * e2);

  // Commutative properties
  void sortChildrenInPlace(ExpressionOrder order, Context * context, bool canSwapMatrices, bool canContainMatrices);
  Expression squashUnaryHierarchyInPlace();

protected:
  LayoutShape leftLayoutShape() const override { return childAtIndex(0)->leftLayoutShape(); };
  LayoutShape rightLayoutShape() const override { return childAtIndex(numberOfChildren()-1)->rightLayoutShape(); }

  /* With a pool of size < 120k and TreeNode of size 20, a node can't have more
   * than 6144 children which fit in uint16_t. */
  uint16_t m_numberOfChildren;
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
  void mergeSameTypeChildrenInPlace();
  /* allChildrenAreReal returns:
   * - 1 if all children are real
   * - 0 if all non real children are ComplexCartesian
   * - -1 if some chidren are non-real and non ComplexCartesian */
  int allChildrenAreReal(Context * context, bool canContainMatrices = true) const;
protected:
  void sortChildrenInPlace(NAryExpressionNode::ExpressionOrder order, Context * context, bool canSwapMatrices, bool canContainMatrices) {
    node()->sortChildrenInPlace(order, context, canSwapMatrices, canContainMatrices);
  }
  NAryExpressionNode * node() const { return static_cast<NAryExpressionNode *>(Expression::node()); }
  Expression checkChildrenAreRationalIntegersAndUpdate(const ExpressionNode::ReductionContext& reductionContext);
};

}

#endif
