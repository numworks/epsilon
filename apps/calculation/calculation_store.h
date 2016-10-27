#ifndef CALCULATION_CALCULATION_STORE_H
#define CALCULATION_CALCULATION_STORE_H

#include "calculation.h"

namespace Calculation {

class CalculationStore {
public:
  CalculationStore();
  Calculation * calculationAtIndex(int i);
  Calculation * push(Calculation * c);
  void deleteCalculationAtIndex(int i);
  int numberOfCalculations();
private:
  static constexpr int k_maxNumberOfCalculations = 20;
  Calculation * m_start;
  Calculation m_calculations[k_maxNumberOfCalculations];
};

}

#endif
