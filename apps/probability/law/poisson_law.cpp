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

bool PoissonLaw::isContinuous() const {
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
  return -k_displayLeftMarginRatio*xMax();
}

float PoissonLaw::xMax() {
  assert(m_parameter1 != 0);
  return (m_parameter1 + 5.0f*sqrtf(m_parameter1))*(1.0f+k_displayRightMarginRatio);
}

float PoissonLaw::yMin() {
  return - k_displayBottomMarginRatio * yMax();
}

float PoissonLaw::yMax() {
  int maxAbscissa = (int)m_parameter1;
  assert(maxAbscissa >= 0.0f);
  float result = evaluateAtAbscissa(maxAbscissa);
  if (result <= 0.0f) {
    result = 1.0f;
  }
  return result*(1.0f+ k_displayTopMarginRatio);
}

float PoissonLaw::evaluateAtAbscissa(float x) const {
  if (x < 0.0f) {
    return NAN;
  }
  float lResult = -m_parameter1+(int)x*logf(m_parameter1)-lgammaf((int)x+1);
  return expf(lResult);
}

bool PoissonLaw::authorizedValueAtIndex(float x, int index) const {
  if (x <= 0.0f) {
    return false;
  }
  return true;
}

}
