#include "expression_selector.h"
extern "C" {
#include <assert.h>
}

int ExpressionSelector::match(Expression * e, ExpressionMatch * matches) {
  int numberOfMatches = 0;

  // Does the current node match?
  switch(m_match) {
    case ExpressionSelector::Match::Any:
      // Yes, it always does!
      break;
    case ExpressionSelector::Match::Type:
      if (e->type() != m_expressionType) {
        return 0;
      }
      break;
    case ExpressionSelector::Match::Wildcard:
      /* This should not happen as a wildcard should be matched _before_ */
      assert(false);
      break;
  }

  // The current node does match. Let's add it to our matches
  matches[numberOfMatches++] = ExpressionMatch(&e, 1);

  for (int i=0; i<m_numberOfChildren; i++) {
    ExpressionSelector * childSelector = this->child(i);
    Expression * childExpression = e->operand(i);

    if (childSelector->m_match == ExpressionSelector::Match::Wildcard) {
      assert(i == m_numberOfChildren-1); // Wildcards should be the last argument!
      Expression * local_expr[255];
      for (int j=i; j<e->numberOfOperands(); j++) {
        local_expr[j-i] = e->operand(j);
      }
      matches[numberOfMatches++] = ExpressionMatch(local_expr, e->numberOfOperands() - i);
    } else {
      int numberOfChildMatches = childSelector->match(childExpression, (matches+numberOfMatches));
      if (numberOfChildMatches == 0) {
        return 0;
      } else {
        numberOfMatches += numberOfChildMatches;
      }
    }
  }

  return numberOfMatches;
}

// Extrude in a class impossible otherwise ExpressionBuilder is not aggregate
// and cannot be initialized statically
ExpressionSelector * ExpressionSelector::child(int index) {
  assert(index>=0 && index<m_numberOfChildren);
  if (index == 0) {
    return (this+1); // Pointer arithmetics
  } else {
    ExpressionSelector * previousChild = this->child(index-1);
    return previousChild+previousChild->m_numberOfChildren+1; // Pointer arithm.
  }
}
