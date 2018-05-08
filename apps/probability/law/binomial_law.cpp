#include "binomial_law.h"
#include <assert.h>
#include <cmath>

namespace Probability {

BinomialLaw::BinomialLaw() :
  TwoParameterLaw(20.0, 0.5)
{
}

I18n::Message BinomialLaw::title() {
  return I18n::Message::BinomialLaw;
}

Law::Type BinomialLaw::type() const {
  return Type::Binomial;
}

bool BinomialLaw::isContinuous() const {
  return false;
}

I18n::Message BinomialLaw::parameterNameAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return I18n::Message::N;
  } else {
    return I18n::Message::P;
  }
}

I18n::Message BinomialLaw::parameterDefinitionAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return I18n::Message::RepetitionNumber;
  } else {
    return I18n::Message::SuccessProbability;
  }
}

float BinomialLaw::xMin() {
  float min = 0.0f;
  float max = m_parameter1 > 0.0f ? m_parameter1 : 1.0f;
  return min - k_displayLeftMarginRatio * (max - min);
}

float BinomialLaw::xMax() {
  float min = 0.0f;
  float max = m_parameter1;
  if (max <= min) {
    max = min + 1.0f;
  }
  return max + k_displayRightMarginRatio*(max - min);
}

float BinomialLaw::yMin() {
  return -k_displayBottomMarginRatio*yMax();
}

float BinomialLaw::yMax() {
  int maxAbscissa = m_parameter2 < 1.0f ? (m_parameter1+1)*m_parameter2 : m_parameter1;
  float result = evaluateAtAbscissa(maxAbscissa);
  if (result <= 0.0f || std::isnan(result)) {
    result = 1.0f;
  }
  return result*(1.0f+ k_displayTopMarginRatio);
}


bool BinomialLaw::authorizedValueAtIndex(float x, int index) const {
  if (index == 0) {
    if (x != (int)x || x < 0.0f || x > 999.0f) {
      return false;
    }
    return true;
  }
  if (x < 0.0f || x > 1.0f) {
    return false;
  }
  return true;
}

double BinomialLaw::cumulativeDistributiveInverseForProbability(double * probability) {
  if (m_parameter1 == 0.0 && (m_parameter2 == 0.0 || m_parameter2 == 1.0)) {
    return NAN;
  }
  if (*probability >= 1.0) {
    return m_parameter1;
  }
  return Law::cumulativeDistributiveInverseForProbability(probability);
}

double BinomialLaw::rightIntegralInverseForProbability(double * probability) {
  if (m_parameter1 == 0.0 && (m_parameter2 == 0.0 || m_parameter2 == 1.0)) {
    return NAN;
  }
  if (*probability <= 0.0) {
    return m_parameter1;
  }
  return Law::rightIntegralInverseForProbability(probability);
}

template<typename T>
T BinomialLaw::templatedApproximateAtAbscissa(T x) const {
  if (m_parameter1 == 0) {
    if (m_parameter2 == 0 || m_parameter2 == 1) {
      return NAN;
    }
    if (floor(x) == 0) {
      return 1;
    }
    return 0;
  }
  if (m_parameter2 == 1) {
    if (floor(x) == m_parameter1) {
      return 1;
    }
    return 0;
  }
  if (m_parameter2 == 0) {
    if (floor(x) == 0) {
      return 1;
    }
    return 0;
  }
  if (x > m_parameter1) {
    return 0;
  }
  T lResult = std::lgamma(m_parameter1+1) - std::lgamma(std::floor(x)+1) - std::lgamma((T)m_parameter1 - std::floor(x)+1)+
    std::floor(x)*std::log(m_parameter2) + (m_parameter1-std::floor(x))*std::log(1-m_parameter2);
  return std::exp(lResult);
}

}

template float Probability::BinomialLaw::templatedApproximateAtAbscissa(float x) const;
template double Probability::BinomialLaw::templatedApproximateAtAbscissa(double x) const;
