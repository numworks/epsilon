#ifndef PROBABILITE_ONE_PARAMETER_DISTRIBUTION_H
#define PROBABILITE_ONE_PARAMETER_DISTRIBUTION_H

#include "distribution.h"
#include <assert.h>

namespace Probability {

class OneParameterDistribution : public Distribution {
public:
  OneParameterDistribution(float parameterValue) : m_parameter1(parameterValue) {}
  int numberOfParameter() override { return 1; }
  double parameterValueAtIndex(int index) override {
    assert(index == 0);
    return m_parameter1;
  }
  void setParameterAtIndex(float f, int index) override {
    assert(index == 0);
    m_parameter1 = f;
  }
protected:
  double m_parameter1;
};

}

#endif
