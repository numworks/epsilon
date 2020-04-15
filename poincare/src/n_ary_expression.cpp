#include <poincare/n_ary_expression.h>
#include <poincare/number.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <utility>

namespace Poincare {

bool NAryExpressionNode::childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const {
  /* Expressions like "-2" require parentheses in Addition/Multiplication except
   * when they are the first operand. */
  if (childIndex != 0
    && ((child.isNumber() && static_cast<const Number &>(child).sign() == Sign::Negative)
      || child.type() == Type::Opposite))
  {
    return true;
  }
  if (child.type() == Type::Conjugate) {
    return childAtIndexNeedsUserParentheses(child.childAtIndex(0), childIndex);
  }
  return false;
}

void NAryExpressionNode::sortChildrenInPlace(ExpressionOrder order, Context * context, bool canSwapMatrices, bool canBeInterrupted) {
  Expression reference(this);
  const int childrenCount = reference.numberOfChildren();
  for (int i = 1; i < childrenCount; i++) {
    bool isSorted = true;
    for (int j = 0; j < childrenCount-1; j++) {
      /* Warning: Matrix operations are not always commutative (ie,
       * multiplication) so we never swap 2 matrices. */
      ExpressionNode * cj = childAtIndex(j);
      ExpressionNode * cj1 = childAtIndex(j+1);
      bool cjIsMatrix = Expression(cj).deepIsMatrix(context);
      bool cj1IsMatrix = Expression(cj1).deepIsMatrix(context);
      bool cj1GreaterThanCj = order(cj, cj1, canBeInterrupted) > 0;
      if ((cjIsMatrix && !cj1IsMatrix) || // we always put matrices at the end of expressions
          (cjIsMatrix && cj1IsMatrix && canSwapMatrices && cj1GreaterThanCj) ||
          (!cjIsMatrix && !cj1IsMatrix && cj1GreaterThanCj)) {
        reference.swapChildrenInPlace(j, j+1);
        isSorted = false;
      }
    }
    if (isSorted) {
      return;
    }
  }
}

Expression NAryExpressionNode::squashUnaryHierarchyInPlace() {
  NAryExpression reference = NAryExpression(this);
  if (reference.numberOfChildren() == 1) {
    Expression child = reference.childAtIndex(0);
    reference.replaceWithInPlace(child);
    return child;
  }
  return std::move(reference);
}

// Private

int NAryExpressionNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const {
  int m = numberOfChildren();
  int n = e->numberOfChildren();
  for (int i = 1; i <= m; i++) {
    // The NULL node is the least node type.
    if (n < i) {
      return 1;
    }
    int order = SimplificationOrder(childAtIndex(m-i), e->childAtIndex(n-i), ascending, canBeInterrupted, ignoreParentheses);
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

int NAryExpressionNode::simplificationOrderGreaterType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const {
  int m = numberOfChildren();
  if (m == 0) {
    return -1;
  }
  /* Compare e to last term of hierarchy. */
  int order = SimplificationOrder(childAtIndex(m-1), e, ascending, canBeInterrupted, ignoreParentheses);
  if (order != 0) {
    return order;
  }
  if (m > 1) {
    return ascending ? 1 : -1;
  }
  return 0;
}

int NAryExpression::allChildrenAreReal(Context * context) const {
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

}
