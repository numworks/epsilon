#ifndef PROBABILITE_RIGHT_INTEGRAL_CALCULATION_H
#define PROBABILITE_RIGHT_INTEGRAL_CALCULATION_H

#include "calculation.h"

namespace Probability {

class RightIntegralCalculation : public Calculation {
public:
  RightIntegralCalculation();
  ~RightIntegralCalculation() override {};
  Type type() const override;
  int numberOfParameters() override;
  const char * legendForParameterAtIndex(int index) override;
  void setParameterAtIndex(float f, int index) override;
  float parameterAtIndex(int index) override;
private:
  void compute(int indexKnownElement) override;
  float m_upperBound;
  float m_result;
};

}

#endif
