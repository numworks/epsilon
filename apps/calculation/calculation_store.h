#ifndef CALCULATION_CALCULATION_STORE_H
#define CALCULATION_CALCULATION_STORE_H

#include "calculation.h"

namespace Calculation {

// TODO: make tests for the ring buffer

class CalculationStore {
public:
  CalculationStore();
  Calculation * calculationAtIndex(int i);
  Calculation * push(const char * text, Poincare::Context * context);
  void deleteCalculationAtIndex(int i);
  void deleteAll();
  int numberOfCalculations();
  void tidy();
private:
  static constexpr int k_maxNumberOfCalculations = 10;
  Calculation * m_start;
  Calculation m_calculations[k_maxNumberOfCalculations];
};

}

#endif
