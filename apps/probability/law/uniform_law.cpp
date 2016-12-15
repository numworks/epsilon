#include "uniform_law.h"
#include <assert.h>

namespace Probability {

UniformLaw::UniformLaw() :
  TwoParameterLaw()
{
}

const char * UniformLaw::title() {
  return "Loi uniforme";
}

Law::Type UniformLaw::type() const {
  return Type::Uniform;
}

bool UniformLaw::isContinuous() {
  return true;
}

const char * UniformLaw::parameterNameAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return "a";
  } else {
    return "b";
  }
}

const char * UniformLaw::parameterDefinitionAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return "[a, b] intervalle";
  } else {
    return nullptr;
  }
}

float UniformLaw::xMin() {
  if (m_parameter2 < m_parameter1) {
    return m_parameter2 - 3.0f;
  }
  return m_parameter1 - 3.0f;
}

float UniformLaw::xMax() {
  if (m_parameter2 < m_parameter1) {
    return m_parameter1 + 3.0f;
  }
  return m_parameter2 + 3.0f;;
}

float UniformLaw::yMin() {
  return -0.2f;
}

float UniformLaw::yMax() {
  return 1.0f;
}

float UniformLaw::evaluateAtAbscissa(float t) const {
  if (m_parameter1 <= t && t <= m_parameter2) {
    return (1.0f/(m_parameter2-m_parameter1));
  }
  return 0.0f;
}

}
