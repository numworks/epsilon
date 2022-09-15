#include "binomial_distribution.h"
#include <poincare/binomial_distribution.h>
#include <poincare/code_point_layout.h>
#include <poincare/layout_helper.h>
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Distributions {

bool BinomialDistribution::authorizedParameterAtIndex(double x, int index) const {
  if (!TwoParametersDistribution::authorizedParameterAtIndex(x, index)) {
    return false;
  }
  if (index == 0) {
    // n must be a positive integer
    return (x == (int)x) && x >= 0.0;
  }
  // p must be between 0 and 1
  return (x >= 0.0) && (x <= 1.0);
}

double BinomialDistribution::cumulativeDistributiveInverseForProbability(double p) const {
  return Poincare::BinomialDistribution::CumulativeDistributiveInverseForProbability(p, m_parameters[0], m_parameters[1]);
}

double BinomialDistribution::rightIntegralInverseForProbability(double p) const {
  if (m_parameters[0] == 0.0 && (m_parameters[1] == 0.0 || m_parameters[1] == 1.0)) {
    return NAN;
  }
  if (p <= 0.0) {
    return m_parameters[0];
  }
  return Distribution::rightIntegralInverseForProbability(p);
}

double BinomialDistribution::evaluateAtDiscreteAbscissa(int k) const {
  return Poincare::BinomialDistribution::EvaluateAtAbscissa<double>((double) k, m_parameters[0], m_parameters[1]);
}

ParameterRepresentation BinomialDistribution::paramRepresentationAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::N:
    {
      Poincare::Layout n = Poincare::LayoutHelper::String(parameterNameAtIndex(ParamsOrder::N));
      return ParameterRepresentation{n, I18n::Message::RepetitionNumber};
    }
    case ParamsOrder::P:
    {
      Poincare::Layout p = Poincare::LayoutHelper::String(parameterNameAtIndex(ParamsOrder::P));
      return ParameterRepresentation{p, I18n::Message::SuccessProbability};
    }
    default:
      assert(false);
      return ParameterRepresentation{};
  }
}

float BinomialDistribution::computeXMin() const {
  float min = 0.0f;
  float max = m_parameters[0] > min ? m_parameters[0] : 1.0f;
  return min - k_displayLeftMarginRatio * (max - min);
}

float BinomialDistribution::computeXMax() const {
  float min = 0.0f;
  float max = m_parameters[0] > min ? m_parameters[0] : 1.0f;
  return max + k_displayRightMarginRatio * (max - min);
}

float BinomialDistribution::computeYMax() const {
  int maxAbscissa = m_parameters[1] < 1.0 ? (m_parameters[0]+1)*m_parameters[1] : m_parameters[0];
  float result = evaluateAtAbscissa(maxAbscissa);
  if (result <= 0.0f || std::isnan(result)) {
    result = 1.0f;
  }
  return result*(1.0f+ k_displayTopMarginRatio);
}

}
