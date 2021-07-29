#include "one_mean_t_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/conjugate_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

using namespace Poincare;

namespace Probability {

OneMeanTStatistic::OneMeanTStatistic() {
  m_params[ParamsOrder::X] = 20;
  m_params[ParamsOrder::N] = 50;
  m_params[ParamsOrder::S] = 2;
  m_hypothesisParams.setFirstParam(128);
}

bool OneMeanTStatistic::isValidParamAtIndex(int i, float p) {
  switch (i) {
    case ParamsOrder::N:
      return p > 0;
    case ParamsOrder::X:
    case ParamsOrder::S:
      return p >= 0;
      break;
  }
  return TStatistic::isValidParamAtIndex(i, p);
}

void OneMeanTStatistic::computeTest() {
  float mean = m_hypothesisParams.firstParam();
  m_degreesOfFreedom = _degreesOfFreedom(n());
  m_zAlpha = absIfNeeded(_tAlpha(m_degreesOfFreedom, m_threshold));
  m_z = _t(mean, x(), s(), n());
  m_pValue = _pVal(m_degreesOfFreedom, m_z);
}

void OneMeanTStatistic::computeInterval() {
  m_pEstimate = x();
  m_degreesOfFreedom = _degreesOfFreedom(n());
  m_zCritical = _tCritical(m_degreesOfFreedom, threshold());
  m_SE = _SE(s(), n());
  m_ME = _ME(m_zCritical, m_SE);
}

ParameterRepr OneMeanTStatistic::paramReprAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X: {
      HorizontalLayout x = HorizontalLayout::Builder(
          ConjugateLayout::Builder(CodePointLayout::Builder('x')));
      return ParameterRepr{x, I18n::Message::SampleMean};
    }
    case ParamsOrder::S: {
      HorizontalLayout n1 = HorizontalLayout::Builder(CodePointLayout::Builder('s'));
      return ParameterRepr{n1, I18n::Message::StandardDeviation};
    }
    case ParamsOrder::N: {
      HorizontalLayout n = HorizontalLayout::Builder(CodePointLayout::Builder('n'));
      return ParameterRepr{n, I18n::Message::SampleSize};
    }
  }
  assert(false);
  return ParameterRepr{};
}

float OneMeanTStatistic::_degreesOfFreedom(int n) {
  return n - 1;
}

float OneMeanTStatistic::_t(float mean, float meanSample, float s, float n) {
  return absIfNeeded((meanSample - mean) / (s / sqrt(n)));
}

float OneMeanTStatistic::_SE(float s, float n) {
  return s / sqrt(n);
}

}  // namespace Probability
