#include "one_mean_z_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/conjugate_layout.h>

#include <cmath>

using namespace Poincare;

namespace Probability {

OneMeanZStatistic::OneMeanZStatistic() {
  m_params[ParamsOrder::X] = 127;
  m_params[ParamsOrder::N] = 10;
  m_params[ParamsOrder::Sigma] = 2;
  m_hypothesisParams.setFirstParam(128);
}

bool OneMeanZStatistic::isValidParamAtIndex(int i, float p) {
  switch (i) {
    case ParamsOrder::N:
      return p > 0;
    case ParamsOrder::X:
    case ParamsOrder::Sigma:
      return p >= 0;
  }
  return ZStatistic::isValidParamAtIndex(i, p);
}

void OneMeanZStatistic::setParamAtIndex(int index, float p) {
  if (index == ParamsOrder::N) {
    p = std::round(p);
  }
  ZStatistic::setParamAtIndex(index, p);
}

void OneMeanZStatistic::computeTest() {
  m_testCriticalValue = computeZ(m_hypothesisParams.firstParam(), x(), n(), sigma());
  m_zAlpha = computeZAlpha(m_threshold, m_hypothesisParams.comparisonOperator());
  m_pValue = computePValue(m_testCriticalValue, m_hypothesisParams.comparisonOperator());
}

void OneMeanZStatistic::computeInterval() {
  m_estimate = x();
  m_zCritical = computeIntervalCriticalValue(m_threshold);
  m_SE = computeStandardError(sigma(), n());
  m_marginOfError = computeMarginOfError(m_zCritical, m_SE);
}

ParameterRepresentation OneMeanZStatistic::paramRepresentationAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X: {
      Layout x = ConjugateLayout::Builder(CodePointLayout::Builder('x'));
      return ParameterRepresentation{x, I18n::Message::SampleMean};
    }
    case ParamsOrder::N: {
      Layout n = CodePointLayout::Builder('n');
      return ParameterRepresentation{n, I18n::Message::SampleSize};
    }
    case ParamsOrder::Sigma: {
      Layout sigma = CodePointLayout::Builder(CodePoint(UCodePointGreekSmallLetterSigma));
      return ParameterRepresentation{sigma, I18n::Message::PopulationStd};
    }
  }
  assert(false);
  return ParameterRepresentation{};
}

float OneMeanZStatistic::computeZ(float mean, float meanSample, float n, float sigma) {
  return (meanSample - mean) / (sigma / sqrt(n));
}

float OneMeanZStatistic::computeStandardError(float sigma, float n) {
  return sigma / sqrt(n);
}

}  // namespace Probability
