#include "one_mean_z_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>

#include <cmath>

using namespace Poincare;

namespace Probability {

void OneMeanZStatistic::init(Data::SubApp subapp) {
  if (subapp == Data::SubApp::Tests) {
    m_params[ParamsOrder::X] = 47.9;
    m_params[ParamsOrder::N] = 12;
    m_params[ParamsOrder::Sigma] = 2.81;
    m_hypothesisParams.setFirstParam(50);
    m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Lower);
  } else {
    m_params[ParamsOrder::X] = 240.80;
    m_params[ParamsOrder::N] = 16;
    m_params[ParamsOrder::Sigma] = 20;
  }
}

bool OneMeanZStatistic::isValidParamAtIndex(int i, double p) {
  switch (i) {
    case ParamsOrder::N:
      return p >= 1.0;
    case ParamsOrder::X:
    case ParamsOrder::Sigma:
      return p > 0.0;
  }
  return ZStatistic::isValidParamAtIndex(i, p);
}

void OneMeanZStatistic::setParamAtIndex(int index, double p) {
  if (index == ParamsOrder::N) {
    p = std::round(p);
    assert(p > 0.0);
  }
  ZStatistic::setParamAtIndex(index, p);
}

void OneMeanZStatistic::computeTest() {
  m_testCriticalValue = computeZ(m_hypothesisParams.firstParam(), x(), n(), sigma());
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
      Layout x = CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline);
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

double OneMeanZStatistic::computeZ(double mean, double meanSample, double n, double sigma) {
  return (meanSample - mean) / (sigma / std::sqrt(n));
}

double OneMeanZStatistic::computeStandardError(double sigma, double n) {
  return sigma / std::sqrt(n);
}

}  // namespace Probability
