#include "one_mean_z_statistic.h"

#include <cmath>

#include "probability/app.h"

namespace Probability {

OneMeanZStatistic::OneMeanZStatistic() {
  m_params[ParamsOrder::X] = 20;
  m_params[ParamsOrder::N] = 50;
  m_params[ParamsOrder::Sigma] = 2;
}

void OneMeanZStatistic::computeTest() {
  m_z = _z(m_hypothesisParams.firstParam(), x(), n(), sigma());
  m_zAlpha = _zAlpha(m_threshold);
  char op = static_cast<char>(m_hypothesisParams.op());
  m_pValue = _pVal(m_z, op);
}

void OneMeanZStatistic::computeInterval() {
  m_pEstimate = x();
  m_zCritical = _zCritical(threshold());
  m_SE = _SE(sigma(), n());
  m_ME = _ME(m_zCritical, m_SE);
}

const ParameterRepr * OneMeanZStatistic::paramReprAtIndex(int i) const {
  constexpr static ParameterRepr params[k_numberOfParams] = {
      {I18n::Message::MeanSymbol, I18n::Message::SampleMean},
      {I18n::Message::N, I18n::Message::SampleSize},
      {I18n::Message::Sigma, I18n::Message::StandardDeviation}};
  return &(params[i]);
}

float OneMeanZStatistic::_z(float mean, float meanSample, float n, float sigma) {
  return (meanSample - mean) / (sigma / sqrt(n));
}

float OneMeanZStatistic::_SE(float sigma, int n) {
  return sigma / sqrt(n);
}

}  // namespace Probability
