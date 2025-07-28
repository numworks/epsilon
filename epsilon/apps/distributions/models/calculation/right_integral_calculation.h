#ifndef PROBABILITE_RIGHT_INTEGRAL_CALCULATION_H
#define PROBABILITE_RIGHT_INTEGRAL_CALCULATION_H

#include "calculation.h"

namespace Distributions {

class RightIntegralCalculation final : public Calculation {
 public:
  RightIntegralCalculation(Distribution* distribution);
  Type type() override { return Type::RightIntegral; }
  int numberOfParameters() override { return 2; }
  I18n::Message legendForParameterAtIndex(int index) override;
  void setParameterAtIndex(double f, int index) override;
  double parameterAtIndex(int index) override;
  double lowerBound() const override { return m_lowerBound; }

 private:
  void compute(int indexKnownElement) override;
  void computeUnknownDistributionParameter() override;
  double m_lowerBound;
  double m_result;
};

}  // namespace Distributions

#endif
