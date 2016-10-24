#include "calculation_store.h"
#include <assert.h>

namespace Calculation {

CalculationStore::CalculationStore() :
  m_numberOfCalculations(0),
  m_startIndex(0)
{
}

Calculation * CalculationStore::calculationAtIndex(int i) {
  assert(i>=0 && i<m_numberOfCalculations);
  int index = (m_startIndex+i) - m_numberOfCalculations*(int)((m_startIndex+i)/m_numberOfCalculations);
  return &m_calculations[index];
}

Calculation * CalculationStore::addEmptyCalculation() {
  //TODO: change the model
  if (m_numberOfCalculations >= k_maxNumberOfCalculations) {
    for (int k = 0; k < k_maxNumberOfCalculations; k++) {
      m_calculations[k] = m_calculations[k+1];
    }
    m_numberOfCalculations--;
  }
  Calculation addedCalculation = Calculation();
  m_calculations[m_numberOfCalculations] = addedCalculation;
  Calculation * result = &m_calculations[m_numberOfCalculations];
  m_numberOfCalculations++;
  return result;
}

void CalculationStore::removeCalculation(Calculation * c) {
  int i = 0;
  while (&m_calculations[i] != c && i < m_numberOfCalculations) {
    i++;
  }
  assert(i>=0 && i<m_numberOfCalculations);
  m_numberOfCalculations--;
  for (int j = i; j<m_numberOfCalculations; j++) {
    m_calculations[j] = m_calculations[j+1];
  }
}

int CalculationStore::numberOfCalculations() {
  return m_numberOfCalculations;
}

}
