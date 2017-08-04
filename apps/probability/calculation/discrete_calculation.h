#ifndef PROBABILITE_DISCRETE_CALCULATION_H
#define PROBABILITE_DISCRETE_CALCULATION_H

#include "calculation.h"

namespace Probability {

class DiscreteCalculation : public Calculation {
public:
  DiscreteCalculation();
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
  double m_abscissa;
  double m_result;
};

}

#endif
