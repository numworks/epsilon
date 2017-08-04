#ifndef PROBABILITE_LEFT_INTEGRAL_CALCULATION_H
#define PROBABILITE_LEFT_INTEGRAL_CALCULATION_H

#include "calculation.h"

namespace Probability {

class LeftIntegralCalculation : public Calculation {
public:
  LeftIntegralCalculation();
  Type type() override;
  int numberOfParameters() override;
  I18n::Message legendForParameterAtIndex(int index) override;
  void setParameterAtIndex(double f, int index) override;
  double parameterAtIndex(int index) override;
  double upperBound() override;
private:
  void compute(int indexKnownElement) override;
  double m_upperBound;
  double m_result;
};

}

#endif
