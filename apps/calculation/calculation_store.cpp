#include "calculation_store.h"
#include <assert.h>

namespace Calculation {

CalculationStore::CalculationStore() :
  m_start(m_calculations)
{
}

Calculation * CalculationStore::push(const char * text, Context * context, Preferences * preferences) {
  Calculation * result = m_start;
  m_start++->setContent(text, context, preferences);
  if (m_start >= m_calculations + k_maxNumberOfCalculations) {
    m_start = m_calculations;
  }
  return result;
}

Calculation * CalculationStore::calculationAtIndex(int i) {
  int j = 0;
  Calculation * currentCalc = m_start;
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
  calculationAtIndex(i)->reset();
}

void CalculationStore::deleteAll() {
  m_start = m_calculations;
  Calculation * currentCalc= m_start;
  while (currentCalc < m_calculations + k_maxNumberOfCalculations) {
    if (!currentCalc->isEmpty()) {
      currentCalc->reset();
    }
    currentCalc++;
  }
}

}
