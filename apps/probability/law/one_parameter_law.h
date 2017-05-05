#ifndef PROBABILITE_ONE_PARAMETER_LAW_H
#define PROBABILITE_ONE_PARAMETER_LAW_H

#include "law.h"

namespace Probability {

class OneParameterLaw : public Law {
public:
  OneParameterLaw(float parameterValue);
  int numberOfParameter() override;
  float parameterValueAtIndex(int index) override;
  void setParameterAtIndex(float f, int index) override;
protected:
  float m_parameter1;
};

}

#endif
