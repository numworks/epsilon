#include "one_mean_t_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>

#include <cmath>

using namespace Poincare;

namespace Probability {

void OneMeanTStatistic::init(Data::SubApp subapp) {
  if (subapp == Data::SubApp::Tests) {
    m_params[ParamsOrder::X] = 47.9;
    m_params[ParamsOrder::N] = 12;
    m_params[ParamsOrder::S] = 2.81;
    m_hypothesisParams.setFirstParam(50);
    m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Lower);
  } else {
    m_params[ParamsOrder::X] = 1.2675;
    m_params[ParamsOrder::N] = 40;
    m_params[ParamsOrder::S] = 0.3332;
  }
}

bool OneMeanTStatistic::isValidParamAtIndex(int i, double p) {
  switch (i) {
    case ParamsOrder::N:
      return p >= 2.0;
    case ParamsOrder::X:
    case ParamsOrder::S:
      return p > 0.0;
      break;
  }
  return TStatistic::isValidParamAtIndex(i, p);
}

void OneMeanTStatistic::setParamAtIndex(int index, double p) {
  if (index == ParamsOrder::N) {
    p = std::round(p);
    assert(p > 1.0);
  }
  TStatistic::setParamAtIndex(index, p);
}

void OneMeanTStatistic::computeTest() {
  double mean = m_hypothesisParams.firstParam();
  m_degreesOfFreedom = computeDegreesOfFreedom(n());
  m_testCriticalValue = computeT(mean, x(), s(), n());
  m_pValue = computePValue(m_testCriticalValue, m_hypothesisParams.comparisonOperator());
}

void OneMeanTStatistic::computeInterval() {
  m_estimate = x();
  m_degreesOfFreedom = computeDegreesOfFreedom(n());
  m_zCritical = computeIntervalCriticalValue(m_threshold);
  m_SE = computeStandardError(s(), n());
  m_marginOfError = computeMarginOfError(m_zCritical, m_SE);
}

ParameterRepresentation OneMeanTStatistic::paramRepresentationAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X: {
      Poincare::Layout x = CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline);
      return ParameterRepresentation{x, I18n::Message::SampleMean};
    }
    case ParamsOrder::S: {
      Poincare::Layout s = CodePointLayout::Builder('s');
      return ParameterRepresentation{s, I18n::Message::SampleSTD};
    }
    case ParamsOrder::N: {
      Poincare::Layout n = CodePointLayout::Builder('n');
      return ParameterRepresentation{n, I18n::Message::SampleSize};
    }
  }
  assert(false);
  return ParameterRepresentation{};
}

double OneMeanTStatistic::computeDegreesOfFreedom(double n) {
  return n - 1.0;
}

double OneMeanTStatistic::computeT(double mean, double meanSample, double s, double n) {
  return (meanSample - mean) / (s / std::sqrt(n));
}

double OneMeanTStatistic::computeStandardError(double s, double n) {
  return s / std::sqrt(n);
}

}  // namespace Probability
