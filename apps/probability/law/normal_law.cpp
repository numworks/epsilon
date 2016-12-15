#include "normal_law.h"
#include <assert.h>
#include <math.h>

namespace Probability {

NormalLaw::NormalLaw() :
  TwoParameterLaw(0.0f, 1.0f)
{
}

const char * NormalLaw::title() {
  return "Loi normale";
}

Law::Type NormalLaw::type() const {
  return Type::Normal;
}

bool NormalLaw::isContinuous() {
  return true;
}

const char * NormalLaw::parameterNameAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return "u";
  } else {
    return "o";
  }
}

const char * NormalLaw::parameterDefinitionAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return "u : moyenne";
  } else {
    return "o : ecart-type";
  }
}

float NormalLaw::xMin() {
  if (m_parameter2 == 0.0f) {
    return m_parameter1 - 1.0f;
  }
  return m_parameter1 - 5.0f*fabsf(m_parameter2);
}

float NormalLaw::xMax() {
  if (m_parameter2 == 0.0f) {
    return m_parameter1 + 1.0f;
  }
  return m_parameter1 + 5.0f*fabsf(m_parameter2);
}

float NormalLaw::yMin() {
  float maxAbscissa = m_parameter1;
  float result = k_minMarginFactor*evaluateAtAbscissa(maxAbscissa);
  if (result >= 0.0f) {
    result = k_minMarginFactor;
  }
  return result;
}

float NormalLaw::yMax() {
  float maxAbscissa = m_parameter1;
  float result = k_maxMarginFactor*evaluateAtAbscissa(maxAbscissa);
  if (result <= 0.0f || result == yMin()) {
    result = yMin() + 1.0f;
  }
  return result;
}

float NormalLaw::evaluateAtAbscissa(float x) const {
  if (m_parameter2 == 0.0f) {
    return NAN;
  }
  return (1.0f/(fabsf(m_parameter2)*sqrtf(2.0f*M_PI)))*expf(-0.5f*powf((x-m_parameter1)/m_parameter2,2));
}

bool NormalLaw::authorizedValueAtIndex(float x, int index) const {
  return true;
}

}
