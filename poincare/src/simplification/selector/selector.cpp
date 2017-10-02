#include "selector.h"
#include "combination.h"
#include <assert.h>

namespace Poincare {
namespace Simplification {

bool Selector::acceptsLocationInCombination(const Combination * combination, int location) const {
  // Yes, if no other slot in the combination before me is the same
  for (int i=0; i<location; i++) {
    if (combination->expressionIndexForSelectorIndex(i) == combination->expressionIndexForSelectorIndex(location)) {
      return false;
    }
  }
  return immediateMatch(combination->expressionForSelectorIndex(location));
}

bool Selector::match(const Expression * e, Expression ** captures, int captureLength) const {
  // Test that root selector match root expression
  if (!immediateMatch(e)) {
    return false;
  }
  // Capture the expression if required
  if (m_captureIndex >= 0) {
    assert(m_captureIndex < captureLength);
    //FIXME: No cast
    captures[m_captureIndex] = (Expression *)e;
  }
  /* If the root selector matched the root expression, we still have to test
   * that every selector's child can match an expression's child: we return
   * true if a combination linking every selector child to an expression child
   * does exist. To avoid combinatorial explosion, we rely on the fact that the
   * expression AND the selector are sorted enabling us to skip many useless
   * cases. */
  if (m_numberOfChildren == 0) {
    return true;
  }
  /* If we want a complete match (meaning every expression's child are matched
   * by a selector's child), the number of selector children should equal the
   * number of expression children. And that is the only condition to add to
   * get a total match! */

  if (!m_childrenPartialMatch && m_numberOfChildren != e->numberOfOperands()) {
    return false;
  }
  Combination combination(m_children, m_numberOfChildren, e, e->isCommutative());
  while (combination.next()){
    bool allChildrenMatched = true;
    for (int i=0; i<m_numberOfChildren; i++) {
      const Selector * child = m_children[i];
      const Expression * expression = combination.expressionForSelectorIndex(i);
      if (!child->match(expression, captures, captureLength)) {
        allChildrenMatched = false;
        break;
      }
    }
    if (allChildrenMatched) {
      return true;
    }
  }

  return false;
}

}
}
