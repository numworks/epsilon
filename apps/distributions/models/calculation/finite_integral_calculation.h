#ifndef PROBABILITE_FINITE_INTEGRAL_CALCULATION_H
#define PROBABILITE_FINITE_INTEGRAL_CALCULATION_H

#include "calculation.h"

namespace Distributions {

class FiniteIntegralCalculation : public Calculation {
public:
  FiniteIntegralCalculation(Distribution * distribution);
  Type type() override { return Type::FiniteIntegral; }
  int numberOfParameters() override { return 3; }
  I18n::Message legendForParameterAtIndex(int index) override;
  void setParameterAtIndex(double f, int index) override;
  double parameterAtIndex(int index) override;
  double lowerBound() const override { return m_lowerBound; }
  double upperBound() const override { return m_upperBound; }
private:
  void compute(int indexKnownElement) override;
  double m_lowerBound;
  double m_upperBound;
  double m_result;
};

}

#endif
