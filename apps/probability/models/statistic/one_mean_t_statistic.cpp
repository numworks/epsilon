#include "one_mean_t_statistic.h"

#include <cmath>

namespace Probability {

OneMeanTStatistic::OneMeanTStatistic() {
  m_params[ParamsOrder::X] = 20;
  m_params[ParamsOrder::N] = 50;
  m_params[ParamsOrder::S] = 2;
  m_hypothesisParams.setFirstParam(128);
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

const ParameterRepr * OneMeanTStatistic::paramReprAtIndex(int i) const {
  constexpr static ParameterRepr params[k_numberOfParams] = {
      {I18n::Message::MeanSymbol, I18n::Message::SampleMean},
      {I18n::Message::s, I18n::Message::StandardDeviation},
      {I18n::Message::N, I18n::Message::SampleSize}};
  return &(params[i]);
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
