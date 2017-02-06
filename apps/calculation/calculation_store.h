#ifndef CALCULATION_CALCULATION_STORE_H
#define CALCULATION_CALCULATION_STORE_H

#include "calculation.h"
#include "../preferences.h"

namespace Calculation {

// TODO: make tests for the ring buffer

class CalculationStore {
public:
  CalculationStore();
  Calculation * calculationAtIndex(int i);
  Calculation * push(const char * text, Poincare::Context * context, Preferences * preferences);
  void deleteCalculationAtIndex(int i);
  void deleteAll();
  int numberOfCalculations();
private:
  static constexpr int k_maxNumberOfCalculations = 20;
  Calculation * m_start;
  Calculation m_calculations[k_maxNumberOfCalculations];
};

}

#endif
