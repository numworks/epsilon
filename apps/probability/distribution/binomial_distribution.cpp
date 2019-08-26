#include "binomial_distribution.h"
#include <poincare/binomial_distribution.h>
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

float BinomialDistribution::evaluateAtAbscissa(float x) const {
  return Poincare::BinomialDistribution::EvaluateAtAbscissa<float>(x, m_parameter1, m_parameter2);
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
  return Poincare::BinomialDistribution::CumulativeDistributiveInverseForProbability<double>(*probability, m_parameter1, m_parameter2);
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

double BinomialDistribution::evaluateAtDiscreteAbscissa(int k) const {
  return Poincare::BinomialDistribution::EvaluateAtAbscissa<double>((double) k, (double)m_parameter1, (double)m_parameter2);
}

}
