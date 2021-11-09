#include "one_proportion_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>

#include <cmath>

using namespace Poincare;

namespace Probability {

void OneProportionStatistic::tidy() {
  m_estimateLayout = Layout();
}

void OneProportionStatistic::init(Data::SubApp subapp) {
  if (subapp == Data::SubApp::Tests) {
    m_params[ParamsOrder::X] = 47;
    m_params[ParamsOrder::N] = 500;
    m_hypothesisParams.setFirstParam(0.08);
    m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Higher);
  } else {
    m_params[ParamsOrder::X] = 107;
    m_params[ParamsOrder::N] = 251;
  }
}

bool OneProportionStatistic::isValidParamAtIndex(int i, double p) {
  switch (i) {
    case ParamsOrder::X:
      return p >= 0.0;
    case ParamsOrder::N:
      return p >= 1.0;
  }
  return ZStatistic::isValidParamAtIndex(i, p);
}

void OneProportionStatistic::computeTest() {
  double p0 = m_hypothesisParams.firstParam();
  double p = computeEstimate(x(), n());

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
  if (m_estimateLayout.isUninitialized()) {
    m_estimateLayout = CombinedCodePointsLayout::Builder('p', UCodePointCombiningCircumflex);
  }
  return m_estimateLayout;
}

void OneProportionStatistic::setParamAtIndex(int index, double p) {
  if (index == ParamsOrder::N || index == ParamsOrder::X) {
    p = std::round(p);
    assert(p > 0.0);
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

double OneProportionStatistic::computeEstimate(double x, double n) {
  return x / n;
}

double OneProportionStatistic::computeZ(double p0, double p, double n) {
  assert(n > 0 && p0 >= 0 && p >= 0 && p0 <= 1 && p <= 1);
  return (p - p0) / std::sqrt(p0 * (1 - p0) / n);
}

double OneProportionStatistic::computeStandardError(double pEstimate, double n) {
  return std::sqrt(pEstimate * (1 - pEstimate) / n);
}

}  // namespace Probability
