#ifndef PROBABILITE_FINITE_INTEGRAL_CALCULATION_H
#define PROBABILITE_FINITE_INTEGRAL_CALCULATION_H

#include "calculation.h"

namespace Probability {

class FiniteIntegralCalculation : public Calculation {
public:
  FiniteIntegralCalculation();
  ~FiniteIntegralCalculation() override {};
  int numberOfParameters() override;
  const char * legendForParameterAtIndex(int index) override;
  void setParameterAtIndex(float f, int index) override;
  float parameterAtIndex(int index) override;
  float lowerBound() override;
  float upperBound() override;
private:
  void compute(int indexKnownElement) override;
  float m_lowerBound;
  float m_upperBound;
  float m_result;
};

}

#endif
