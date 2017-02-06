extern "C" {
#include <assert.h>
#include <stdlib.h>
}

#include <poincare/integer.h>
#include <poincare/symbol.h>
#include "expression_selector.h"

namespace Poincare {

const uint8_t kUnmatched = 0xff;

int ExpressionSelector::numberOfNonWildcardChildren() {
  bool hasWildcard = child(m_numberOfChildren-1)->m_match == ExpressionSelector::Match::Wildcard;
  int numberOfChildren = m_numberOfChildren;
  if (hasWildcard) {
    numberOfChildren--;
  }
  return numberOfChildren;
}

int ExpressionSelector::match(const Expression * e, ExpressionMatch * matches) {
  return this->match(e, matches, 0);
}

int ExpressionSelector::match(const Expression * e, ExpressionMatch * matches, int offset) {
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
    case ExpressionSelector::Match::TypeAndValue:
      if (e->type() != m_expressionType) {
        return 0;
      }
      switch(e->type()) {
        case Expression::Type::Integer:
          {
            Integer integer = Integer(m_integerValue);
            if (!e->valueEquals(&integer)) {
              return 0;
            }
          }
          break;
        case Expression::Type::Symbol:
          {
            Symbol symbol = Symbol(m_symbolName);
            if (!e->valueEquals(&symbol)) {
              return 0;
            }
          }
          break;
        default:
          // Symbol and Integer are the only expressions which should be matched
          // with a value.
          assert(false);
          break;
      }
      break;
    case ExpressionSelector::Match::Wildcard:
      /* This should not happen as a wildcard should be matched _before_ */
      assert(false);
      break;
    case ExpressionSelector::Match::SameAs:
      /* Here we assume that the match can only have a single child, as we
       * don't want to match on wildcards. */
      assert(matches[m_integerValue].numberOfExpressions() == 1);
      if (!e->isEquivalentTo((Expression *)matches[m_sameAsPosition].expression(0))) {
        return 0;
      }
      break;
  }

  // The current node does match. Let's add it to our matches
  matches[offset + numberOfMatches++] = ExpressionMatch(&e, 1);

  if (m_numberOfChildren != 0) {
    int numberOfChildMatches = 0;
    if (!e->isCommutative()) {
      numberOfChildMatches = sequentialMatch(e, matches, offset+numberOfMatches);
    } else {
      numberOfChildMatches = commutativeMatch(e, matches, offset+numberOfMatches);
    }
    // We check whether the children matched or not.
    if (numberOfChildMatches == 0) {
      return 0;
    } else {
      numberOfMatches += numberOfChildMatches;
    }
  }

  return numberOfMatches;
}

/* This tries to match the children selector sequentialy */
int ExpressionSelector::sequentialMatch(const Expression * e,
    ExpressionMatch * matches, int offset) {
  int numberOfMatches = 0;
  for (int i=0; i<m_numberOfChildren; i++) {
    ExpressionSelector * childSelector = child(i);
    const Expression * childExpression = e->operand(i);

    if (childSelector->m_match == ExpressionSelector::Match::Wildcard) {
      assert(false); // There should not be a wildcard for non commutative op.
    } else {
      int numberOfChildMatches = childSelector->match(
          childExpression,
          matches,
          offset+numberOfMatches);
      if (numberOfChildMatches == 0) {
        return 0;
      } else {
        numberOfMatches += numberOfChildMatches;
      }
    }
  }
  return numberOfMatches;
}

/* This iterates over the combinations of possible matches in the children of
 * a selector and then writes the output ExpressionMatch to matches just like
 * match would do.
 */
int ExpressionSelector::commutativeMatch(const Expression * e, ExpressionMatch * matches, int offset) {
  // If we have more children to match than the expression has, we cannot match.
  if (e->numberOfOperands() < m_numberOfChildren) {
    return 0;
  }

  bool hasWildcard = child(m_numberOfChildren-1)->m_match == ExpressionSelector::Match::Wildcard;
  uint8_t * selectorMatched = (uint8_t *)malloc(e->numberOfOperands()*sizeof(uint8_t));

  /* Initialize the selectors matched to unmatched (0xff), here we assume that
   * we never have more than 255 direct children selector. */
  assert(this->m_numberOfChildren<kUnmatched);
  for (int i(0); i<e->numberOfOperands(); i++) {
    selectorMatched[i] = kUnmatched;
  }

  /* TODO: This one's size can be determined by the compiler as this is the
   * maximum number of direct children selectors. */
  uint8_t expressionMatched[5];

  /* If we have a wildcard we do not want to try to match it to an expression
   * yet. */
  int numberOfChildren = this->numberOfNonWildcardChildren();

  if (!canCommutativelyMatch(e, matches, selectorMatched, numberOfChildren, offset)) {
    free(selectorMatched);
    return 0;
  }

  /* Note that we need this indirection because the order in which the selectors
   * are defined is significant.
   *
   * We can see that in the following example:
   * Here we try to match:
   *     +
   *    / \
   *   +   w
   *  /
   * w
   * Onto:
   *     +
   *    / \
   *   4   +
   *      / \
   *     2   3
   *
   * Since + is commutative they do match which is nice.
   * The issue here is that the matches are expected in a certain order by the
   * builder, here the order expected is:
   *     0
   *    / \
   *   1   3
   *  /
   * 2
   * But The nodes of the expressions are matched in this order:
   *     0
   *    / \
   *   1   2
   *        \
   *         3
   * Which is not what we want, thus remembering which selector matched which
   * expression allows us to put the match in the proper order.
   * We got an expression -> selector relationship, here we are just inversting
   * it to get the selector -> expression relationship that we want. */
  for (int i = 0; i<e->numberOfOperands(); i++) {
    if (selectorMatched[i] != kUnmatched) {
      expressionMatched[selectorMatched[i]] = i;
    }
  }

  /* Here we recursively write the matches of each selector in the matches
   * table.
   *
   * Using the example in the previous comment we would write
   * | + | + | (Integer(2),Integer(3)) | Integer(4) |
   *
   * The pointer arithmetic with numberOfMatches, allows us to know how many
   * matches a selector has written.
   * Using the previous example, the part with
   * +
   * |
   * w
   * would write two matches:
   * + and (Integer(2), Integer(3))
   * whereas:
   * w
   * would only write one:
   * Integer(4)  */
  int numberOfMatches = 0;
  for (int i(0); i<numberOfChildren; i++) {
    int numberOfChildMatches = child(i)->match(
        e->operand(expressionMatched[i]),
        matches,
        offset + numberOfMatches);
    assert(numberOfChildMatches > 0);
    numberOfMatches += numberOfChildMatches;
  }

  if (hasWildcard) {
    /* We allocate a table of Expression* the size of the number of unmatched
     * operands. */
    const Expression ** local_expr = (const Expression**) malloc((e->numberOfOperands() - numberOfChildren) * sizeof(Expression*));
    int j = 0;
    for (int i(0); i<e->numberOfOperands(); i++) {
      // if the expression was not matched, give it to the wildcard.
      if (selectorMatched[i] == kUnmatched) {
        local_expr[j++] = e->operand(i);
      }
    }
    matches[offset + numberOfMatches++] = ExpressionMatch(local_expr, j);
    free(local_expr);
  }

  free(selectorMatched);
  return numberOfMatches;
}

/* This helper function finds whether there is a commutative match or not.
 * The output is the selector matched for each expression, written in the
 * selectorMatched table.
 * The matches table will be written to, but will not overwrite any previous
 * data so this is ok.
 * leftToMatch tells it how many selectors still have to be matched.
 * Implementation detail: selectors are matched in ascending order.
 */
bool ExpressionSelector::canCommutativelyMatch(const Expression * e,
    ExpressionMatch * matches,
    uint8_t * selectorMatched,
    int leftToMatch,
    int offset) {
  bool hasWildcard = child(m_numberOfChildren-1)->m_match == ExpressionSelector::Match::Wildcard;

  // This part is used to make sure that we stop once we matched everything.
  if (leftToMatch == 0) {
    if (hasWildcard) {
      return true;
    } else {
      /* We check that we matched all the children expressions.
       * This can happen if the selector does not have a wildcard and we matched all
       * the selectors, but there are still unmatched children expression.
       * An example is the expression:
       *   +
       * / | \
       * 1 2 3
       * Which would otherwise be matched by
       *    +
       *   / \
       *  /   \
       * Int Int
       */
      int matchedChildren = 0;
      for (int i = 0; i<e->numberOfOperands(); i++) {
        if (selectorMatched[i] != kUnmatched) {
          matchedChildren++;
        }
      }
      if (matchedChildren == e->numberOfOperands()) {
        return true;
      } else {
        return false;
      }
    }
  }

  // We try to match the i-th child selector.
  int i = numberOfNonWildcardChildren() - leftToMatch;
  for (int j = 0; j<e->numberOfOperands(); j++) {
    // If the child has already been matched, we skip it.
    if (selectorMatched[j] != kUnmatched) {
      continue;
    }
    int numberOfMatches = child(i)->match(e->operand(j), matches, offset);
    if (numberOfMatches) {
      // We managed to match this selector.
      selectorMatched[j] = i;
      /* We check that we can match the rest in this configuration, if so we
       * are good. */
      if (this->canCommutativelyMatch(e, matches, selectorMatched, leftToMatch - 1, offset + numberOfMatches)) {
        return true;
      }
      // Otherwise we backtrack.
      selectorMatched[j] = kUnmatched;
    }
  }

  return false;
}

/* Extrude in a class impossible otherwise ExpressionBuilder is not aggregate
 * and cannot be initialized statically. */
ExpressionSelector * ExpressionSelector::child(int index) {
  assert(index>=0 && index<m_numberOfChildren);
  if (index == 0) {
    return (this+1); // Pointer arithmetics
  } else {
    ExpressionSelector * previousChild = this->child(index-1);
    return previousChild+previousChild->m_numberOfChildren+1; // Pointer arithm.
  }
}

}
