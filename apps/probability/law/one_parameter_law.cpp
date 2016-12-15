#include "one_parameter_law.h"
#include <assert.h>

namespace Probability {

OneParameterLaw::OneParameterLaw(float parameterValue) :
  m_parameter1(parameterValue)
{
}

int OneParameterLaw::numberOfParameter() {
  return 1;
}

float OneParameterLaw::parameterValueAtIndex(int index) {
  assert(index == 0);
  return m_parameter1;
}

void OneParameterLaw::setParameterAtIndex(float f, int index) {
  assert(index == 0);
  m_parameter1 = f;
}

}
