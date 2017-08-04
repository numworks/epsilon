#ifndef PROBABILITE_RIGHT_INTEGRAL_CALCULATION_H
#define PROBABILITE_RIGHT_INTEGRAL_CALCULATION_H

#include "calculation.h"

namespace Probability {

class RightIntegralCalculation : public Calculation {
public:
  RightIntegralCalculation();
  Type type() override;
  int numberOfParameters() override;
  I18n::Message legendForParameterAtIndex(int index) override;
  void setParameterAtIndex(double f, int index) override;
  double parameterAtIndex(int index) override;
  double lowerBound() override;
private:
  void compute(int indexKnownElement) override;
  double m_lowerBound;
  double m_result;
};

}

#endif
