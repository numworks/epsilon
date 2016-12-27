#include "two_parameter_law.h"
#include <assert.h>

namespace Probability {

TwoParameterLaw::TwoParameterLaw(float parameterValue1, float parameterValue2) :
  m_parameter1(parameterValue1),
  m_parameter2(parameterValue2)
{
}

int TwoParameterLaw::numberOfParameter() {
  return 2;
}

float TwoParameterLaw::parameterValueAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_parameter1;
  }
  return m_parameter2;
}

void TwoParameterLaw::setParameterAtIndex(float f, int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    m_parameter1 = f;
  } else {
    m_parameter2 = f;
  }
}

}
