#include <poincare/n_ary_infix_expression.h>
#include <poincare/number.h>

namespace Poincare {

bool NAryInfixExpressionNode::childAtIndexNeedsUserParentheses(
    const Expression &child, int childIndex) const {
  /* Expressions like "-2" require parentheses in Addition/Multiplication except
   * when they are the first operand. (same for -2%) */
  if (childIndex != 0 &&
      ((child.isNumber() && static_cast<const Number &>(child).isPositive() ==
                                TrinaryBoolean::False) ||
       child.type() == Type::Opposite)) {
    return true;
  }
  if (child.type() == Type::Conjugate || child.type() == Type::PercentSimple) {
    return childAtIndexNeedsUserParentheses(child.childAtIndex(0), childIndex);
  }
  return false;
}

int NAryInfixExpressionNode::simplificationOrderSameType(
    const ExpressionNode *e, bool ascending, bool ignoreParentheses) const {
  int m = numberOfChildren();
  int n = e->numberOfChildren();
  for (int i = 1; i <= m; i++) {
    // The NULL node is the least node type.
    if (n < i) {
      return 1;
    }
    int order = SimplificationOrder(childAtIndex(m - i), e->childAtIndex(n - i),
                                    ascending, ignoreParentheses);
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

int NAryInfixExpressionNode::simplificationOrderGreaterType(
    const ExpressionNode *e, bool ascending, bool ignoreParentheses) const {
  int m = numberOfChildren();
  if (m == 0) {
    return -1;
  }
  /* Compare e to last term of hierarchy. */
  int order =
      SimplificationOrder(childAtIndex(m - 1), e, ascending, ignoreParentheses);
  if (order != 0) {
    return order;
  }
  if (m > 1) {
    return ascending ? 1 : -1;
  }
  return 0;
}

}  // namespace Poincare
