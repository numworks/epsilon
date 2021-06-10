#include "one_proportion_statistic.h"

#include <poincare/normal_distribution.h>

#include <cmath>

#include "probability/app.h"

namespace Probability {

OneProportionStatistic::OneProportionStatistic() {
  m_params[ParamsOrder::X] = 20;
  m_params[ParamsOrder::N] = 50;
}

void OneProportionStatistic::computeTest() {
  float p0 = m_hypothesisParams.firstParam();
  float prop = _pEstimate(x(), n());

  m_zAlpha = _zAlpha(m_threshold);
  m_z = _z(p0, prop, n());
  char fakeOp = static_cast<char>(m_hypothesisParams.op());
  m_pValue = _pVal(m_z, fakeOp);
}

void OneProportionStatistic::computeInterval() {
  m_pEstimate = _pEstimate(x(), n());
  m_zAlpha = _zAlpha(m_threshold);
  m_zCritical = _zCritical(threshold());
  m_SE = _SE(m_pEstimate, n());
  m_ME = _ME(m_zCritical, m_SE);
}

const ParameterRepr * OneProportionStatistic::paramReprAtIndex(int i) const {
  constexpr static ParameterRepr s_paramReprs[k_numberOfParams] = {
      {I18n::Message::X, I18n::Message::NumberOfSuccesses},
      {I18n::Message::N, I18n::Message::SampleSize}};
  return &s_paramReprs[i];
}

float OneProportionStatistic::_pEstimate(float x, float n) {
  return x / n;
}

float OneProportionStatistic::_z(float p0, float p, int n) {
  return (p - p0) / sqrt(p0 * (1 - p0) / n);
}

float OneProportionStatistic::_SE(float pEstimate, int n) {
  return sqrt(pEstimate * (1 - pEstimate) / n);
}

}  // namespace Probability
