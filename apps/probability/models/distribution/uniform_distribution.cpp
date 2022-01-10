#include "uniform_distribution.h"
#include <cmath>
#include <float.h>
#include <assert.h>
#include <algorithm>

namespace Probability {


I18n::Message UniformDistribution::parameterNameAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return I18n::Message::A;
  } else {
    return I18n::Message::B;
  }
}

I18n::Message UniformDistribution::parameterDefinitionAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return I18n::Message::IntervalADescr;
  } else {
    return I18n::Message::IntervalBDescr;
  }
}

float UniformDistribution::xMin() const {
  assert(m_parameter2 >= m_parameter1);
  if (m_parameter2 - m_parameter1 < FLT_EPSILON) {
    // If parameter is too big, subtracting only 1.0 wouldn't do anything.
    return m_parameter1 - std::max(1.0, std::fabs(m_parameter1) * k_displayLeftMarginRatio);
  }
  return m_parameter1 - 0.6f * (m_parameter2 - m_parameter1);
}

float UniformDistribution::xMax() const {
  if (m_parameter2 - m_parameter1 < FLT_EPSILON) {
    // If parameter is too big, adding only 1.0 wouldn't do anything.
    return m_parameter1 + std::max(1.0, std::fabs(m_parameter1) * k_displayRightMarginRatio);
  }
  return m_parameter2 + 0.6f * (m_parameter2 - m_parameter1);
}

float UniformDistribution::yMax() const {
  float result = m_parameter2 - m_parameter1 < FLT_EPSILON ? k_diracMaximum : 1.0f/(m_parameter2-m_parameter1);
  if (result <= 0.0f || std::isnan(result) || std::isinf(result)) {
    result = 1.0f;
  }
  return result * (1.0f+ k_displayTopMarginRatio);
}

float UniformDistribution::evaluateAtAbscissa(float t) const {
  float parameter1 = m_parameter1;
  float parameter2 = m_parameter2;
  if (parameter2 - parameter1 < FLT_EPSILON) {
    if (parameter1 - k_diracWidth<= t && t <= parameter2 + k_diracWidth) {
      return 2.0f * k_diracMaximum;
    }
    return 0.0f;
  }
  if (parameter1 <= t && t <= parameter2) {
    return (1.0f/(parameter2 - parameter1));
  }
  return 0.0f;
}

bool UniformDistribution::authorizedValueAtIndex(double x, int index) const {
  if (!TwoParameterDistribution::authorizedValueAtIndex(x, index)) {
    return false;
  }
  if (index == 0) {
    return true;
  }
  if (m_parameter1 > x) {
    return false;
  }
  return true;
}

void UniformDistribution::setParameterAtIndex(double f, int index) {
  TwoParameterDistribution::setParameterAtIndex(f, index);
  if (index == 0 && m_parameter2 < m_parameter1) {
    // Add more than 1.0 if first parameter is greater than 100.
    m_parameter2 = m_parameter1 + std::max(1.0, std::round(std::fabs(m_parameter1) * 0.01));
  }
}

double UniformDistribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  if (x <= m_parameter1) {
    return 0.0;
  }
  if (x < m_parameter2) {
    return (x - m_parameter1)/(m_parameter2 - m_parameter1);
  }
  return 1.0;
}

double UniformDistribution::cumulativeDistributiveInverseForProbability(double * probability) {
  if (*probability >= 1.0f) {
    return m_parameter2;
  }
  if (*probability <= 0.0f) {
    return m_parameter1;
  }
  return m_parameter1 * (1 - *probability) + *probability * m_parameter2;
}

}
