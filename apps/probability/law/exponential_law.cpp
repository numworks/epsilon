#include "exponential_law.h"
#include <assert.h>
#include <math.h>
#include <float.h>

namespace Probability {

ExponentialLaw::ExponentialLaw() :
  OneParameterLaw(1.0f)
{
}

const char * ExponentialLaw::title() {
  return "Loi exponentielle";
}

Law::Type ExponentialLaw::type() const {
  return Type::Exponential;
}

bool ExponentialLaw::isContinuous() const {
  return true;
}

const char * ExponentialLaw::parameterNameAtIndex(int index) {
  assert(index == 0);
  return "l";
}

const char * ExponentialLaw::parameterDefinitionAtIndex(int index) {
  assert(index == 0);
  return "l : parametre";
}

float ExponentialLaw::xMin() {
  return 0.0f;
}

float ExponentialLaw::xMax() {
  assert(m_parameter1 != 0.0f);
  float result = 5.0f/m_parameter1;
  if (result <= xMin()) {
    result = xMin() + 1.0f;
  }
  return result;
}

float ExponentialLaw::yMin() {
  return 0.0f;
}

float ExponentialLaw::yMax() {
  float result = m_parameter1;
  if (result <= yMin()) {
    result = yMin() + 1.0f;
  }
  return result;
}

float ExponentialLaw::evaluateAtAbscissa(float x) const {
  if (x < 0.0f) {
    return NAN;
  }
  return m_parameter1*expf(-m_parameter1*x);
}

bool ExponentialLaw::authorizedValueAtIndex(float x, int index) const {
  if (x <= 0.0f) {
    return false;
  }
  return true;
}

float ExponentialLaw::cumulativeDistributiveFunctionAtAbscissa(float x) const {
  return 1.0f - expf(-m_parameter1*x);
}

float ExponentialLaw::cumulativeDistributiveInverseForProbability(float * probability) {
  if (*probability >= 1.0f) {
    return INFINITY;
  }
  if (*probability <= 0.0f) {
    return 0.0f;
  }
  return -logf(1.0f - *probability)/m_parameter1;
}

}
