#include "two_parameter_distribution.h"
#include <assert.h>

namespace Probability {

double TwoParameterDistribution::parameterValueAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_parameter1;
  }
  return m_parameter2;
}

void TwoParameterDistribution::protectedSetParameterAtIndex(double f, int i) {
  assert(i >= 0 && i < 2);
  if (i == 0) {
    m_parameter1 = f;
  } else {
    m_parameter2 = f;
  }
}

void TwoParameterDistribution::setParameterAtIndex(double f, int i) {
  protectedSetParameterAtIndex(f, i);
  computeCurveViewRange();
}

}
