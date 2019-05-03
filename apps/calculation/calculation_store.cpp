#include "calculation_store.h"
#include <assert.h>
#include <poincare/rational.h>
#include <poincare/symbol.h>
using namespace Poincare;

namespace Calculation {

Calculation * CalculationStore::push(const char * text, Context * context) {
  Calculation * result = &m_calculations[m_startIndex];
  result->setContent(text, context, ansExpression(context));
  m_startIndex++;
  if (m_startIndex >= k_maxNumberOfCalculations) {
    m_startIndex = 0;
  }
  return result;
}

Calculation * CalculationStore::calculationAtIndex(int i) {
  int j = 0;
  Calculation * currentCalc = &m_calculations[m_startIndex];
  Calculation * previousCalc = nullptr;
  while (j <= i) {
    if (!currentCalc++->isEmpty()) {
      previousCalc = currentCalc - 1;
      j++;
    }
    if (currentCalc >= m_calculations + k_maxNumberOfCalculations) {
      currentCalc = m_calculations;
    }
  }
  return previousCalc;
}

int CalculationStore::numberOfCalculations() {
  Calculation * currentCalc= m_calculations;
  int numberOfCalculations = 0;
  while (currentCalc < m_calculations + k_maxNumberOfCalculations) {
    if (!currentCalc++->isEmpty()) {
      numberOfCalculations++;
    }
  }
  return numberOfCalculations;
}

void CalculationStore::deleteCalculationAtIndex(int i) {
  int numberOfCalc = numberOfCalculations();
  assert(i >= 0 && i < numberOfCalc);
  int indexFirstCalc = m_startIndex;
  while (m_calculations[indexFirstCalc].isEmpty()) {
    indexFirstCalc++;
    if (indexFirstCalc == k_maxNumberOfCalculations) {
      indexFirstCalc = 0;
    }
    assert(indexFirstCalc != m_startIndex);
  }
  int absoluteIndexCalculationI = indexFirstCalc+i;
  absoluteIndexCalculationI = absoluteIndexCalculationI >= k_maxNumberOfCalculations ? absoluteIndexCalculationI - k_maxNumberOfCalculations : absoluteIndexCalculationI;

  int index = absoluteIndexCalculationI;
  for (int k = i; k < numberOfCalc-1; k++) {
    int nextIndex = index+1 >= k_maxNumberOfCalculations ? 0 : index+1;
    m_calculations[index] = m_calculations[nextIndex];
    index++;
    if (index == k_maxNumberOfCalculations) {
      index = 0;
    }
  }
  m_calculations[index].reset();
  m_startIndex--;
  if (m_startIndex == -1) {
    m_startIndex = k_maxNumberOfCalculations-1;
  }
}

void CalculationStore::deleteAll() {
  m_startIndex = 0;
  for (int i = 0; i < k_maxNumberOfCalculations; i++) {
    m_calculations[i].reset();
  }
}

void CalculationStore::tidy() {
  for (int i = 0; i < k_maxNumberOfCalculations; i++) {
    m_calculations[i].tidy();
  }
}

Expression CalculationStore::ansExpression(Context * context) {
  if (numberOfCalculations() == 0) {
    return Rational::Builder(0);
  }
  Calculation * lastCalculation = calculationAtIndex(numberOfCalculations()-1);
  /* Special case: the exact output is a Store/Equal expression.
   * Store/Equal expression can only be at the root of an expression.
   * To avoid turning 'ans->A' in '2->A->A' or '2=A->A' (which cannot be
   * parsed), ans is replaced by the approximation output when any Store or
   * Equal expression appears. */
  bool exactOuptutInvolvesStoreEqual = lastCalculation->exactOutput().recursivelyMatches([](const Expression e, Context & context) {
          return e.type() == ExpressionNode::Type::Store || e.type() == ExpressionNode::Type::Equal;
        }, *context, false);
  if (lastCalculation->input().recursivelyMatches(Expression::IsApproximate, *context) || exactOuptutInvolvesStoreEqual) {
    return lastCalculation->approximateOutput(context);
  }
  return lastCalculation->exactOutput();
}

}
