#ifndef POINCARE_N_ARY_EXPRESSION_H
#define POINCARE_N_ARY_EXPRESSION_H

#include "complex_cartesian.h"
#include "old_expression.h"

namespace Poincare {

class NAryExpressionNode
    : public ExpressionNode {  // TODO: VariableArityExpressionNode?
 public:
#if ASSERTIONS
  void setChildrenInPlace(OExpression other) override { assert(false); }
#endif

  // Tree
  int numberOfChildren() const override { return m_numberOfChildren; }
  void setNumberOfChildren(int numberOfChildren) override {
    m_numberOfChildren = numberOfChildren;
  }

  // Comparison
  typedef int (*ExpressionOrder)(const ExpressionNode* e1,
                                 const ExpressionNode* e2);

  // Commutative properties
  void sortChildrenInPlace(ExpressionOrder order, Context* context,
                           bool canSwapMatrices, bool canContainMatrices);
  OExpression squashUnaryHierarchyInPlace();

 protected:
  LayoutShape leftLayoutShape() const override {
    return childAtIndex(0)->leftLayoutShape();
  };
  LayoutShape rightLayoutShape() const override {
    return childAtIndex(numberOfChildren() - 1)->rightLayoutShape();
  }

  /* With a pool of size < 120k and PoolObject of size 20, a node can't have
   * more than 6144 children which fit in uint16_t. */
  uint16_t m_numberOfChildren;
};

class NAryExpression : public OExpression {
 public:
  NAryExpression(const NAryExpressionNode* n) : OExpression(n) {}
  using PoolHandle::addChildAtIndexInPlace;
  using PoolHandle::removeChildAtIndexInPlace;
  using PoolHandle::removeChildInPlace;
  OExpression squashUnaryHierarchyInPlace() {
    return node()->squashUnaryHierarchyInPlace();
  }
  void mergeSameTypeChildrenInPlace();

 protected:
  void sortChildrenInPlace(NAryExpressionNode::ExpressionOrder order,
                           Context* context, bool canSwapMatrices,
                           bool canContainMatrices) {
    node()->sortChildrenInPlace(order, context, canSwapMatrices,
                                canContainMatrices);
  }
  NAryExpressionNode* node() const {
    return static_cast<NAryExpressionNode*>(OExpression::node());
  }
  using ComplexOperator = ComplexCartesian (ComplexCartesian::*)(
      ComplexCartesian& other, const ReductionContext& reductionContext);
  // /!\ Only call this during reduction.
  OExpression combineComplexCartesians(ComplexOperator complexOperator,
                                       ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
