#include "poisson_law.h"
#include <assert.h>
#include <math.h>

namespace Probability {

PoissonLaw::PoissonLaw() :
  OneParameterLaw(4.0f)
{
}

const char * PoissonLaw::title() {
  return "Loi Poisson";
}

Law::Type PoissonLaw::type() const {
  return Type::Poisson;
}

bool PoissonLaw::isContinuous() {
  return false;
}

const char * PoissonLaw::parameterNameAtIndex(int index) {
  assert(index == 0);
  return "l";
}

const char * PoissonLaw::parameterDefinitionAtIndex(int index) {
  assert(index == 0);
  return "l : parametre";
}

float PoissonLaw::xMin() {
  return 0.0f;
}

float PoissonLaw::xMax() {
  assert(m_parameter1 != 0);
  return m_parameter1 + 5.0f*sqrtf(m_parameter1);
}

float PoissonLaw::yMin() {
  int maxAbscissa = (int)m_parameter1;
  return k_minMarginFactor*evaluateAtAbscissa(maxAbscissa);
}

float PoissonLaw::yMax() {
  int maxAbscissa = (int)m_parameter1;
  return k_maxMarginFactor*evaluateAtAbscissa(maxAbscissa);
}

float PoissonLaw::evaluateAtAbscissa(float x) const {
  float lResult = -m_parameter1+(int)x*logf(m_parameter1)-lgammaf((int)x+1);
  return expf(lResult);
}

}
