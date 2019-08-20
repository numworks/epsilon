#ifndef PROBABILITE_TWO_PARAMETER_DISTRIBUTION_H
#define PROBABILITE_TWO_PARAMETER_DISTRIBUTION_H

#include "distribution.h"

namespace Probability {

class TwoParameterDistribution : public Distribution {
public:
  TwoParameterDistribution(float parameterValue1, float parameterValue2) :
    m_parameter1(parameterValue1),
    m_parameter2(parameterValue2)
  {}
  int numberOfParameter() override { return 2; }
  float parameterValueAtIndex(int index) override;
  void setParameterAtIndex(float f, int index) override;
protected:
  float m_parameter1;
  float m_parameter2;
};

}

#endif
