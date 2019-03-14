#ifndef PROBABILITE_ONE_PARAMETER_LAW_H
#define PROBABILITE_ONE_PARAMETER_LAW_H

#include "law.h"
#include <assert.h>

namespace Probability {

class OneParameterLaw : public Law {
public:
  OneParameterLaw(float parameterValue) : m_parameter1(parameterValue) {}
  int numberOfParameter() override { return 1; }
  float parameterValueAtIndex(int index) override {
    assert(index == 0);
    return m_parameter1;
  }
  void setParameterAtIndex(float f, int index) override {
    assert(index == 0);
    m_parameter1 = f;
  }
protected:
  float m_parameter1;
};

}

#endif
