#include "binomial_distribution.h"
#include <assert.h>
#include <cmath>

namespace Probability {

I18n::Message BinomialDistribution::parameterNameAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return I18n::Message::N;
  } else {
    return I18n::Message::P;
  }
}

I18n::Message BinomialDistribution::parameterDefinitionAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return I18n::Message::RepetitionNumber;
  } else {
    return I18n::Message::SuccessProbability;
  }
}

float BinomialDistribution::xMin() const {
  float min = 0.0f;
  float max = m_parameter1 > 0.0f ? m_parameter1 : 1.0f;
  return min - k_displayLeftMarginRatio * (max - min);
}

float BinomialDistribution::xMax() const {
  float min = 0.0f;
  float max = m_parameter1;
  if (max <= min) {
    max = min + 1.0f;
  }
  return max + k_displayRightMarginRatio*(max - min);
}

float BinomialDistribution::yMax() const {
  int maxAbscissa = m_parameter2 < 1.0f ? (m_parameter1+1)*m_parameter2 : m_parameter1;
  float result = evaluateAtAbscissa(maxAbscissa);
  if (result <= 0.0f || std::isnan(result)) {
    result = 1.0f;
  }
  return result*(1.0f+ k_displayTopMarginRatio);
}


bool BinomialDistribution::authorizedValueAtIndex(float x, int index) const {
  if (index == 0) {
    /* As the cumulative probability are computed by looping over all discrete
     * abscissa within the interesting range, the complexity of the cumulative
     * probability is linear with the size of the range. Here we cap the maximal
     * size of the range to 10000. If one day we want to increase or get rid of
     *  this cap, we should implement the explicit formula of the cumulative
     *  probability (which depends on an incomplete beta function) to make the
     *  comlexity O(1). */
    if (x != (int)x || x < 0.0f || x > 99999.0f) {
      return false;
    }
    return true;
  }
  if (x < 0.0f || x > 1.0f) {
    return false;
  }
  return true;
}

double BinomialDistribution::cumulativeDistributiveInverseForProbability(double * probability) {
  if (m_parameter1 == 0.0 && (m_parameter2 == 0.0 || m_parameter2 == 1.0)) {
    return NAN;
  }
  if (*probability >= 1.0) {
    return m_parameter1;
  }
  return Distribution::cumulativeDistributiveInverseForProbability(probability);
}

double BinomialDistribution::rightIntegralInverseForProbability(double * probability) {
  if (m_parameter1 == 0.0 && (m_parameter2 == 0.0 || m_parameter2 == 1.0)) {
    return NAN;
  }
  if (*probability <= 0.0) {
    return m_parameter1;
  }
  return Distribution::rightIntegralInverseForProbability(probability);
}

template<typename T>
T BinomialDistribution::templatedApproximateAtAbscissa(T x) const {
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
  T lResult = std::lgamma((T)(m_parameter1+1.0)) - std::lgamma(std::floor(x)+(T)1.0) - std::lgamma((T)m_parameter1 - std::floor(x)+(T)1.0)+
    std::floor(x)*std::log((T)m_parameter2) + ((T)m_parameter1-std::floor(x))*std::log((T)(1.0-m_parameter2));
  return std::exp(lResult);
}

}

template float Probability::BinomialDistribution::templatedApproximateAtAbscissa(float x) const;
template double Probability::BinomialDistribution::templatedApproximateAtAbscissa(double x) const;
