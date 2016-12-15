#include "exponential_law.h"
#include <assert.h>
#include <math.h>

namespace Probability {

ExponentialLaw::ExponentialLaw() :
  OneParameterLaw()
{
}

const char * ExponentialLaw::title() {
  return "Loi exponentielle";
}

Law::Type ExponentialLaw::type() const {
  return Type::Exponential;
}

bool ExponentialLaw::isContinuous() {
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
  if (m_parameter1 == 0.0f) {
    return 100.0f;
  }
  return 5.0f/m_parameter1;
}

float ExponentialLaw::yMin() {
  return -0.2f;
}

float ExponentialLaw::yMax() {
  return 1.0f;
}

float ExponentialLaw::evaluateAtAbscissa(float x) const {
// TODO: change 2.7f for the right constant
  return m_parameter1*powf(2.7f, -m_parameter1*x);
}

}
