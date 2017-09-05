#ifndef CALCULATION_CALCULATION_STORE_H
#define CALCULATION_CALCULATION_STORE_H

#include "calculation.h"

namespace Calculation {

class CalculationStore {
public:
  CalculationStore();
  Calculation * calculationAtIndex(int i);
  Calculation * push(const char * text, Poincare::Context * context);
  void deleteCalculationAtIndex(int i);
  void deleteAll();
  int numberOfCalculations();
  void tidy();
  static constexpr int k_maxNumberOfCalculations = 10;
private:
  int m_startIndex;
  Calculation m_calculations[k_maxNumberOfCalculations];
};

}

#endif
