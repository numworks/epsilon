#ifndef CALCULATION_CALCULATION_STORE_H
#define CALCULATION_CALCULATION_STORE_H

#include "calculation.h"

namespace Calculation {

class CalculationStore {
public:
  CalculationStore();
  Calculation * calculationAtIndex(int i);
  Calculation * addEmptyCalculation();
  void removeCalculation(Calculation * c);
  int numberOfCalculations();
private:
  static constexpr int k_maxNumberOfCalculations = 10;
  int m_numberOfCalculations;
  int m_startIndex;
  Calculation m_calculations[k_maxNumberOfCalculations];
};

}

#endif
