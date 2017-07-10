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
  void setParameterAtIndex(float f, int index) override;
  float parameterAtIndex(int index) override;
  float lowerBound() override;
  float upperBound() override;
private:
  void compute(int indexKnownElement) override;
  float m_abscissa;
  float m_result;
};

}

#endif
