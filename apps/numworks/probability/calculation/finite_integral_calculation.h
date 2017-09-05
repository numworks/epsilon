#ifndef PROBABILITE_FINITE_INTEGRAL_CALCULATION_H
#define PROBABILITE_FINITE_INTEGRAL_CALCULATION_H

#include "calculation.h"

namespace Probability {

class FiniteIntegralCalculation : public Calculation {
public:
  FiniteIntegralCalculation();
  Type type() override;
  int numberOfParameters() override;
  int numberOfEditableParameters() override;
  I18n::Message legendForParameterAtIndex(int index) override;
  void setParameterAtIndex(double f, int index) override;
  double parameterAtIndex(int index) override;
  double lowerBound() override;
  double upperBound() override;
private:
  void compute(int indexKnownElement) override;
  double m_lowerBound;
  double m_upperBound;
  double m_result;
};

}

#endif
