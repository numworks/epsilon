#ifndef PROBABILITE_DISCRETE_CALCULATION_H
#define PROBABILITE_DISCRETE_CALCULATION_H

#include "calculation.h"

namespace Probability {

class DiscreteCalculation final : public Calculation {
public:
  DiscreteCalculation(Distribution * distribution);
  Type type() override { return Type::Discrete; }
  int numberOfParameters() override { return 2; }
  I18n::Message legendForParameterAtIndex(int index) override;
  void setParameterAtIndex(double f, int index) override;
  double parameterAtIndex(int index) override;
  double lowerBound() override { return m_abscissa; }
  double upperBound() override { return m_abscissa; }
private:
  void compute(int indexKnownElement) override;
  double m_abscissa;
  double m_result;
};

}

#endif
