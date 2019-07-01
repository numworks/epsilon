#include <poincare/n_ary_expression.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

void NAryExpressionNode::sortChildrenInPlace(ExpressionOrder order, bool canBeInterrupted) {
  Expression reference(this);
  for (int i = reference.numberOfChildren()-1; i > 0; i--) {
    bool isSorted = true;
    for (int j = 0; j < reference.numberOfChildren()-1; j++) {
      /* Warning: Matrix operations are not always commutative (ie,
       * multiplication) so we never swap 2 matrices. */
#if MATRIX_EXACT_REDUCING
      if (order(childAtIndex(j), childAtIndex(j+1), canBeInterrupted) > 0 && (!childAtIndex(j)->recursivelyMatches(Expression::IsMatrix) || !childAtIndex(j+1)->recursivelyMatches(Expression::IsMatrix))) {
#else
      if (order(childAtIndex(j), childAtIndex(j+1), canBeInterrupted) > 0) {
#endif
        reference.swapChildrenInPlace(j, j+1);
        isSorted = false;
      }
    }
    if (isSorted) {
      return;
    }
  }
}

bool NAryExpressionNode::isReal(Context & context) const {
  return NAryExpression(this).allChildrenAreReal(context) == 1;
}

Expression NAryExpressionNode::squashUnaryHierarchyInPlace() {
  NAryExpression reference = NAryExpression(this);
  if (reference.numberOfChildren() == 1) {
    Expression child = reference.childAtIndex(0);
    reference.replaceWithInPlace(child);
    return child;
  }
  return reference;
}

// Private

int NAryExpressionNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const {
  int m = numberOfChildren();
  int n = e->numberOfChildren();
  for (int i = 1; i <= m; i++) {
    // The NULL node is the least node type.
    if (n < i) {
      return 1;
    }
    int order = SimplificationOrder(childAtIndex(m-i), e->childAtIndex(n-i), ascending, canBeInterrupted);
    if (order != 0) {
      return order;
    }
  }
  // The NULL node is the least node type.
  if (n > m) {
    return ascending ? -1 : 1;
  }
  return 0;
}

int NAryExpressionNode::simplificationOrderGreaterType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const {
  int m = numberOfChildren();
  if (m == 0) {
    return -1;
  }
  /* Compare e to last term of hierarchy. */
  int order = SimplificationOrder(childAtIndex(m-1), e, ascending, canBeInterrupted);
  if (order != 0) {
    return order;
  }
  if (m > 1) {
    return ascending ? 1 : -1;
  }
  return 0;
}

int NAryExpression::allChildrenAreReal(Context & context) const {
  int i = 0;
  int result = 1;
  while (i < numberOfChildren()) {
    Expression c = childAtIndex(i);
    if (c.type() == ExpressionNode::Type::ComplexCartesian) {
      result *= 0;
    } else if (!c.isReal(context)) {
      return -1;
    }
    i++;
  }
  return result;
}

bool NAryExpression::SortedIsMatrix(Expression e, Context & context) {
  assert(IsNAry(e, context));
  int childrenCount = e.numberOfChildren();
  if (childrenCount > 0) {
     return SortedIsMatrix(e.childAtIndex(childrenCount - 1), context);
  }
  return false;
}

}
