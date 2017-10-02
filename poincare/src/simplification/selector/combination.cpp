#include "combination.h"
#include <assert.h>

namespace Poincare {
namespace Simplification {

Combination::Combination(const Selector * const * selectors, int numberOfSelectors, const Expression * rootExpression, bool enableCrossings) :
  m_selectors(selectors),
  m_numberOfSelectors(numberOfSelectors),
  m_rootExpression(rootExpression),
  m_firstIteration(true),
  m_enableCrossings(enableCrossings)
{
  assert(m_numberOfSelectors < 255);
}

#if 0
void Combination::print() const {
  std::cout << "(";
  for (int i=0; i<m_numberOfSelectors; i++) {
    if (i != 0) {
      std::cout << ", ";
    }
    std::cout << expressionIndexForSelectorIndex(i);
  }
  std::cout << ")" << std::endl;
}
#endif

bool Combination::expressionIndexForSelectorIndexIsValidAtDigit(int digit) const {
  int index = m_expressionIndexForSelectorIndex[digit];
  if (index < 0 || index >= m_rootExpression->numberOfOperands()) {
    return false;
  }
  return m_selectors[digit]->acceptsLocationInCombination(this, digit);
}

bool Combination::next() {
  int digit = m_firstIteration ? 0 : m_numberOfSelectors-1;
  resetExpressionIndexForSelectorIndex(digit);
  bool forceOneIncrementation = !m_firstIteration;
  m_firstIteration = false;

  while (digit >= 0) {
    if (!forceOneIncrementation && expressionIndexForSelectorIndexIsValidAtDigit(digit)) { // Of course this is forwarded to Selectors/Expression
      // Good, go "deeper"
      if (digit < (m_numberOfSelectors-1)) {
        digit++;
        resetExpressionIndexForSelectorIndex(digit);
      } else {
        return true;
      }
    } else {
      m_expressionIndexForSelectorIndex[digit]++;
      forceOneIncrementation = false;
      if (m_expressionIndexForSelectorIndex[digit] >= m_rootExpression->numberOfOperands()) {
        resetExpressionIndexForSelectorIndex(digit);
        digit--;
        forceOneIncrementation = true;
      }
    }
  }
  return false;
}

void Combination::resetExpressionIndexForSelectorIndex(int digit) {
  m_expressionIndexForSelectorIndex[digit] = m_enableCrossings || digit < 1 ? 0 : m_expressionIndexForSelectorIndex[digit-1]+1;
}
}
}
