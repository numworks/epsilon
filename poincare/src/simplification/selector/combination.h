#ifndef POINCARE_SIMPLIFICATION_SELECTOR_COMBINATION_H
#define POINCARE_SIMPLIFICATION_SELECTOR_COMBINATION_H

#include <poincare/expression.h>
#include "selector.h"

namespace Poincare {
namespace Simplification {

/* Combination is the association of two tables: a selectors table and an expressions
 * table.
 * The aim is to find combination between both:
 *
 * Selectors:   s1 s2 s3 s4 ... s10
 *               |   \ /   \ /    |
 *               |    x     x     |
 *               |   / \   / \    |
 * Expressions: e1 e2 e3 e4 ... e10
 *
 * Crossing between selectors and expressions can be enable or disable.
 * */

class Combination {
public:
  Combination(const Selector * const * selectors, int numberOfSelectors, const Expression * rootExpression, bool enableCrossings);
  int expressionIndexForSelectorIndex(int i) const { return m_expressionIndexForSelectorIndex[i]; }
  const Expression * expressionForSelectorIndex(int i) const { return m_rootExpression->operand(expressionIndexForSelectorIndex(i)); }
  bool next();
  // TODO: delete this method created only for testing purposes
  void print() const;
private:
  bool expressionIndexForSelectorIndexIsValidAtDigit(int digit) const;
  void resetExpressionIndexForSelectorIndex(int digit);
  const Selector * const * m_selectors;
  int m_numberOfSelectors;
  const Expression * m_rootExpression;
  int m_expressionIndexForSelectorIndex[255]; // Hard limit at compile-time. Much lower in fact.
  bool m_firstIteration;
  bool m_enableCrossings;
};

}
}

#endif
