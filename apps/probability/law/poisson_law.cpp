#include "poisson_law.h"
#include <assert.h>
#include <math.h>

namespace Probability {

PoissonLaw::PoissonLaw() :
  OneParameterLaw()
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
  if (m_parameter1 == 0.0f) {
    return 1.0f;
  }
  return m_parameter1 + 5.0f*sqrtf(m_parameter1);
}

float PoissonLaw::yMin() {
  return -0.2f;
}

float PoissonLaw::yMax() {
  return 1.0f;
}

float PoissonLaw::evaluateAtAbscissa(float x) const {
  // TODO: 2.7f -> e and factio
  return powf(2.7f, -m_parameter1)*powf(m_parameter1, x)/(x);
}

}
