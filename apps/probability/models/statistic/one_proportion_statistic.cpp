#include "one_proportion_statistic.h"

#include <poincare/code_point_layout.h>

#include <cmath>

#include "probability/app.h"

using namespace Poincare;

namespace Probability {

OneProportionStatistic::OneProportionStatistic() {
  if (App::app()->subapp() == Data::SubApp::Tests) {
    m_params[ParamsOrder::X] = 47;
    m_params[ParamsOrder::N] = 500;
    m_hypothesisParams.setFirstParam(0.08);
    m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Higher);
  } else {
    m_params[ParamsOrder::X] = 107;
    m_params[ParamsOrder::N] = 251;

  }
}

bool OneProportionStatistic::isValidParamAtIndex(int i, float p) {
  switch (i) {
    case ParamsOrder::X:
      return p >= 0;
    case ParamsOrder::N:
      return p > 0;
  }
  return ZStatistic::isValidParamAtIndex(i, p);
}

void OneProportionStatistic::computeTest() {
  float p0 = m_hypothesisParams.firstParam();
  float p = computeEstimate(x(), n());

  m_zAlpha = computeZAlpha(m_threshold, m_hypothesisParams.comparisonOperator());
  m_testCriticalValue = computeZ(p0, p, n());
  m_pValue = computePValue(m_testCriticalValue, m_hypothesisParams.comparisonOperator());
}

void OneProportionStatistic::computeInterval() {
  m_estimate = computeEstimate(x(), n());
  m_zCritical = computeIntervalCriticalValue(m_threshold);
  m_SE = computeStandardError(m_estimate, n());
  m_marginOfError = computeMarginOfError(m_zCritical, m_SE);
}

Poincare::Layout OneProportionStatistic::estimateLayout() {
  return CodePointLayout::Builder('p');  // TODO replace with ^p
}

void OneProportionStatistic::setParamAtIndex(int index, float p) {
  if (index == ParamsOrder::N || index == ParamsOrder::X) {
    p = std::round(p);
  }
  ZStatistic::setParamAtIndex(index, p);
}

ParameterRepresentation OneProportionStatistic::paramRepresentationAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X: {
      Layout x = CodePointLayout::Builder('x');
      return ParameterRepresentation{x, I18n::Message::NumberOfSuccesses};
    }
    case ParamsOrder::N: {
      Layout n = CodePointLayout::Builder('n');
      return ParameterRepresentation{n, I18n::Message::SampleSize};
    }
  }
  assert(false);
  return ParameterRepresentation{};
}

bool OneProportionStatistic::validateInputs() {
  return x() <= n();
}

float OneProportionStatistic::computeEstimate(float x, float n) {
  return x / n;
}

float OneProportionStatistic::computeZ(float p0, float p, float n) {
  assert(n > 0 && p0 >= 0 && p >= 0 && p0 <= 1 && p <= 1);
  return (p - p0) / sqrt(p0 * (1 - p0) / n);
}

float OneProportionStatistic::computeStandardError(float pEstimate, float n) {
  return sqrt(pEstimate * (1 - pEstimate) / n);
}

}  // namespace Probability
