#ifndef PROBABILITE_LEFT_INTEGRAL_CALCULATION_H
#define PROBABILITE_LEFT_INTEGRAL_CALCULATION_H

#include "calculation.h"

namespace Distributions {

class LeftIntegralCalculation final : public Calculation {
public:
  LeftIntegralCalculation(Distribution * distribution);
  Type type() override { return Type::LeftIntegral; }
  int numberOfParameters() override { return 2; }
  I18n::Message legendForParameterAtIndex(int index) override;
  void setParameterAtIndex(double f, int index) override;
  double parameterAtIndex(int index) override;
  double upperBound() const override { return m_upperBound; }
private:
  void compute(int indexKnownElement) override;
  double m_upperBound;
  double m_result;
};

}

#endif
