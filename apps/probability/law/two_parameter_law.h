#ifndef PROBABILITE_TWO_PARAMETER_LAW_H
#define PROBABILITE_TWO_PARAMETER_LAW_H

#include "law.h"

namespace Probability {

class TwoParameterLaw : public Law {
public:
  TwoParameterLaw(float parameterValue1, float parameterValue2);
  int numberOfParameter() override;
  float parameterValueAtIndex(int index) override;
  void setParameterAtIndex(float f, int index) override;
protected:
  float m_parameter1;
  float m_parameter2;
};

}

#endif
