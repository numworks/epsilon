#include "one_mean_t_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/conjugate_layout.h>
#include <poincare/horizontal_layout.h>

#include <cmath>

using namespace Poincare;

namespace Probability {

OneMeanTStatistic::OneMeanTStatistic() {
  m_params[ParamsOrder::X] = 127;
  m_params[ParamsOrder::N] = 10;
  m_params[ParamsOrder::S] = 2;
  m_hypothesisParams.setFirstParam(128);
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

void OneMeanTStatistic::computeTest() {
  float mean = m_hypothesisParams.firstParam();
  m_degreesOfFreedom = computeDegreesOfFreedom(n());
  m_zAlpha = computeZAlpha(m_threshold, m_hypothesisParams.op());
  m_testCriticalValue = computeT(mean, x(), s(), n());
  m_pValue = computePValue(m_testCriticalValue, m_hypothesisParams.op());
}

void OneMeanTStatistic::computeInterval() {
  m_estimate = x();
  m_degreesOfFreedom = computeDegreesOfFreedom(n());
  m_zCritical = computeIntervalCriticalValue(m_threshold);
  m_SE = computeStandardError(s(), n());
  m_marginOfError = computeMarginOfError(m_zCritical, m_SE);
}

ParameterRepr OneMeanTStatistic::paramReprAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X: {
      HorizontalLayout x = HorizontalLayout::Builder(
          ConjugateLayout::Builder(CodePointLayout::Builder('x')));
      return ParameterRepr{x, I18n::Message::SampleMean};
    }
    case ParamsOrder::S: {
      HorizontalLayout s = HorizontalLayout::Builder(CodePointLayout::Builder('s'));
      return ParameterRepr{s, I18n::Message::StandardDeviation};
    }
    case ParamsOrder::N: {
      HorizontalLayout n = HorizontalLayout::Builder(CodePointLayout::Builder('n'));
      return ParameterRepr{n, I18n::Message::SampleSize};
    }
  }
  assert(false);
  return ParameterRepr{};
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
