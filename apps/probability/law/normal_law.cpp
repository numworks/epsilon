#include "normal_law.h"
#include <assert.h>
#include <math.h>

namespace Probability {

NormalLaw::NormalLaw() :
  TwoParameterLaw()
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
  return m_parameter1 - 5.0f*m_parameter2;
}

float NormalLaw::xMax() {
  if (m_parameter2 == 0.0f) {
    return m_parameter1 + 1.0f;
  }
  return m_parameter1 + 5.0f*m_parameter2;
}

float NormalLaw::yMin() {
  return -0.2f;
}

float NormalLaw::yMax() {
  return 1.0f;
}

float NormalLaw::evaluateAtAbscissa(float x) const {
  // TODO: 3.14f -> Pi and 2.7f -> e
  return (1.0f/(m_parameter2*sqrtf(2*3.14f)))*powf(2.7f, -0.5f*powf((x-m_parameter1)/m_parameter2,2));
}

}
