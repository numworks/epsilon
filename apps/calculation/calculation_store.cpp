#include "calculation_store.h"
#include <assert.h>

namespace Calculation {

CalculationStore::CalculationStore() :
  m_start(m_calculations)
{
}

Calculation * CalculationStore::push(Calculation * c) {
  Calculation * result = m_start;
  *m_start++ = *c;
  if (m_start >= m_calculations + k_maxNumberOfCalculations) {
    m_start = m_calculations;
  }
  return result;
}

Calculation * CalculationStore::calculationAtIndex(int i) {
  int j = 0;
  Calculation * currentCalc = m_start;
  while (j<=i) {
    if (currentCalc >= m_calculations + k_maxNumberOfCalculations) {
      currentCalc = m_calculations;
    }
    if (!currentCalc->isEmpty()) {
      j++;
    }
    currentCalc++;
  }
  return currentCalc-1;
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
  *calculationAtIndex(i) = Calculation();
}

}
