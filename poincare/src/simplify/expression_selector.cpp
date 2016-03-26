#include "expression_selector.h"
extern "C" {
#include <assert.h>
}

int ExpressionSelector::match(Expression * e, Expression ** matches) {
  int numberOfMatches = 0;

  // Does the current node match?
  switch(m_match) {
    case ExpressionSelector::Match::Any:
      // Yes, it always does!
      break;
    case ExpressionSelector::Match::TypeAndValue:
      if (e->type() != m_expressionType) {
        return 0;
      }
      /*
      switch (e->type()) {
        case Expression::Type::Integer:
          // Test e->equals(Integer(m_integerValue));
          // Maybe return 0
          break;
        case Expression::Type::Symbol:
          // Test symbol is same name
          // Maybe return 0
          break;
        default:
          // By default we don't check a value equality
      }
      */
      break;
  }

  // The current node does match. Let's add it to our matches
  matches[numberOfMatches++] = e;

  // FIXME: For now we'll ignore the commutativity of the Selector
  // which is definitely something *very* important
  // Checking if "e" is commutative seems like a nice solution

  for (int i=0; i<m_numberOfChildren; i++) {
    ExpressionSelector * childSelector = this->child(i);
    // To account for commutativity, we should have multiple possibilities for childExpression
    Expression * childExpression = e->operand(i);
    int numberOfChildMatches = childSelector->match(childExpression, &matches[numberOfMatches]);
    if (numberOfChildMatches == 0) {
      return 0;
    } else {
      numberOfMatches += numberOfChildMatches;
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
    return previousChild+previousChild->m_numberOfChildren; // Pointer arithm.
  }
}
