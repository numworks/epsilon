#include "uniform_law.h"
#include <assert.h>

namespace Probability {

UniformLaw::UniformLaw() :
  TwoParameterLaw(-1.0f, 1.0f)
{
}

const char * UniformLaw::title() {
  return "Loi uniforme";
}

Law::Type UniformLaw::type() const {
  return Type::Uniform;
}

bool UniformLaw::isContinuous() const {
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
  assert(m_parameter2 >= m_parameter1);
  return m_parameter1 - 0.6f*(m_parameter2 - m_parameter1);
}

float UniformLaw::xMax() {
  return m_parameter2 + 0.6f*(m_parameter2 - m_parameter1);
}

float UniformLaw::yMin() {
  return 0.0f;
}

float UniformLaw::yMax() {
  return (1.0f/(m_parameter2-m_parameter1));
}

float UniformLaw::evaluateAtAbscissa(float t) const {
  if (m_parameter1 <= t && t <= m_parameter2) {
    return (1.0f/(m_parameter2-m_parameter1));
  }
  return 0.0f;
}

bool UniformLaw::authorizedValueAtIndex(float x, int index) const {
  if (index == 0) {
    if (x > m_parameter2) {
      return false;
    }
    return true;
  }
  if (m_parameter1 > x) {
    return false;
  }
  return true;
}

float UniformLaw::cumulativeDistributiveFunctionAtAbscissa(float x) const {
  if (x < m_parameter1) {
    return 0.0f;
  }
  if (x < m_parameter2) {
    return (x-m_parameter1)/(m_parameter2-m_parameter1);
  }
  return 1.0f;
}

float UniformLaw::cumulativeDistributiveInverseForProbability(float * probability) {
  if (*probability >= 1.0f) {
    return m_parameter2;
  }
  if (*probability <= 0.0f) {
    return m_parameter1;
  }
  return m_parameter1*(1-*probability)+*probability*m_parameter2;
}

}
