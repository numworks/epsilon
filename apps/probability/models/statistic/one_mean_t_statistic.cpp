#include "one_mean_t_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/conjugate_layout.h>
#include <poincare/horizontal_layout.h>

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

bool OneMeanTStatistic::isValidParamAtIndex(int i, float p) {
  switch (i) {
    case ParamsOrder::N:
      return p > 1;
    case ParamsOrder::X:
    case ParamsOrder::S:
      return p >= 0;
      break;
  }
  return TStatistic::isValidParamAtIndex(i, p);
}

void OneMeanTStatistic::setParamAtIndex(int index, float p) {
  if (index == ParamsOrder::N) {
    p = std::round(p);
  }
  TStatistic::setParamAtIndex(index, p);
}

void OneMeanTStatistic::computeTest() {
  float mean = m_hypothesisParams.firstParam();
  m_degreesOfFreedom = computeDegreesOfFreedom(n());
  m_zAlpha = computeZAlpha(m_threshold, m_hypothesisParams.comparisonOperator());
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
      Poincare::HorizontalLayout x = Poincare::HorizontalLayout::Builder(
          ConjugateLayout::Builder(CodePointLayout::Builder('x')));
      return ParameterRepresentation{x, I18n::Message::SampleMean};
    }
    case ParamsOrder::S: {
      Poincare::HorizontalLayout s = Poincare::HorizontalLayout::Builder(
          CodePointLayout::Builder('s'));
      return ParameterRepresentation{s, I18n::Message::SampleSTD};
    }
    case ParamsOrder::N: {
      Poincare::HorizontalLayout n = Poincare::HorizontalLayout::Builder(
          CodePointLayout::Builder('n'));
      return ParameterRepresentation{n, I18n::Message::SampleSize};
    }
  }
  assert(false);
  return ParameterRepresentation{};
}

float OneMeanTStatistic::computeDegreesOfFreedom(float n) {
  return n - 1;
}

float OneMeanTStatistic::computeT(float mean, float meanSample, float s, float n) {
  return (meanSample - mean) / (s / sqrt(n));
}

float OneMeanTStatistic::computeStandardError(float s, float n) {
  return s / sqrt(n);
}

}  // namespace Probability
