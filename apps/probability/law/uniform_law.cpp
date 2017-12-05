#include "uniform_law.h"
#include <cmath>
#include <float.h>
#include <assert.h>

namespace Probability {

UniformLaw::UniformLaw() :
  TwoParameterLaw(-1.0f, 1.0f)
{
}

I18n::Message UniformLaw::title() {
  return I18n::Message::UniformLaw;
}

Law::Type UniformLaw::type() const {
  return Type::Uniform;
}

bool UniformLaw::isContinuous() const {
  return true;
}

I18n::Message UniformLaw::parameterNameAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return I18n::Message::A;
  } else {
    return I18n::Message::B;
  }
}

I18n::Message UniformLaw::parameterDefinitionAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return I18n::Message::IntervalDefinition;
  } else {
    return I18n::Message::Default;
  }
}

float UniformLaw::xMin() {
  assert(m_parameter2 >= m_parameter1);
  if (m_parameter2 - m_parameter1 < FLT_EPSILON) {
    return m_parameter1 - 1.0f;
  }
  return m_parameter1 - 0.6f*(m_parameter2 - m_parameter1);
}

float UniformLaw::xMax() {
  if (m_parameter2 - m_parameter1 < FLT_EPSILON) {
    return m_parameter1 + 1.0f;
  }
  return m_parameter2 + 0.6f*(m_parameter2 - m_parameter1);
}

float UniformLaw::yMin() {
  return -k_displayBottomMarginRatio*yMax();
}

float UniformLaw::yMax() {
  float result = m_parameter2 - m_parameter1 < FLT_EPSILON ? k_diracMaximum : 1.0f/(m_parameter2-m_parameter1);
  if (result <= 0.0f || std::isnan(result) || std::isinf(result)) {
    result = 1.0f;
  }
  return result*(1.0f+ k_displayTopMarginRatio);
}

float UniformLaw::evaluateAtAbscissa(float t) const {
  if (m_parameter2 - m_parameter1 < FLT_EPSILON) {
    if (m_parameter1 - k_diracWidth<= t && t <= m_parameter2 + k_diracWidth) {
      return 2.0f*k_diracMaximum;
    }
    return 0.0f;
  }
  if (m_parameter1 <= t && t <= m_parameter2) {
    return (1.0f/(m_parameter2-m_parameter1));
  }
  return 0.0f;
}

bool UniformLaw::authorizedValueAtIndex(float x, int index) const {
  if (index == 0) {
    return true;
  }
  if (m_parameter1 > x) {
    return false;
  }
  return true;
}

void UniformLaw::setParameterAtIndex(float f, int index) {
  TwoParameterLaw::setParameterAtIndex(f, index);
  if (index == 0 && m_parameter2 < m_parameter1) {
    m_parameter2 = m_parameter1+1.0f;
  }
}

double UniformLaw::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  if (x <= m_parameter1) {
    return 0.0;
  }
  if (x < m_parameter2) {
    return (x-m_parameter1)/(m_parameter2-m_parameter1);
  }
  return 1.0;
}

double UniformLaw::cumulativeDistributiveInverseForProbability(double * probability) {
  if (*probability >= 1.0f) {
    return m_parameter2;
  }
  if (*probability <= 0.0f) {
    return m_parameter1;
  }
  return m_parameter1*(1-*probability)+*probability*m_parameter2;
}

}
