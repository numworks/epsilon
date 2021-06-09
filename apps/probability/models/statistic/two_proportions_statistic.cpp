#include "two_proportions_statistic.h"

#include <poincare/normal_distribution.h>

#include <cmath>

#include "probability/app.h"

namespace Probability {

TwoProportionsStatistic::TwoProportionsStatistic() {
  m_params[ParamsOrder::X1] = 20;
  m_params[ParamsOrder::N1] = 50;
  m_params[ParamsOrder::X2] = 40;
  m_params[ParamsOrder::N2] = 60;
}

void TwoProportionsStatistic::computeTest() {
  float deltaP0 = m_hypothesisParams.firstParam();

  m_z = _z(deltaP0, x1(), n1(), x2(), n2());
  char op = static_cast<char>(m_hypothesisParams.op());
  m_pValue = _pVal(m_z, op);
}

void TwoProportionsStatistic::computeInterval() {
  m_pEstimate = _pEstimate(x1(), n1(), x2(), n2());
  m_zCritical = _zCritical(threshold());
  m_SE = _SE(x1(), n1(), x2(), n2());
  m_ME = _ME(m_zCritical, m_SE);
}

const ParameterRepr * TwoProportionsStatistic::paramReprAtIndex(int i) const {
  constexpr static ParameterRepr params[k_numberOfParams] = {
      {I18n::Message::X1, I18n::Message::SuccessSample1},
      {I18n::Message::N1, I18n::Message::Sample1Size},
      {I18n::Message::X2, I18n::Message::SuccessSample2},
      {I18n::Message::N2, I18n::Message::Sample2Size}};
  return &(params[i]);
}

float TwoProportionsStatistic::_pEstimate(float x1, float n1, float x2, float n2) {
  return x1 / n1 - x2 / n2;
}

float TwoProportionsStatistic::_z(float deltaP0, float x1, int n1, float x2, int n2) {
  float p1 = x1 / n1;
  float p2 = x2 / n2;
  float p = (x1 + x2) / (n1 + n2);
  return (p1 - p2 - deltaP0) / sqrt(p * (1 - p) * (1 / n1 + 1 / n2));
}

float TwoProportionsStatistic::_SE(float x1, int n1, float x2, int n2) {
  float p1Estimate = x1 / n1;
  float p2Estimate = x2 / n2;
  return sqrt(p1Estimate * (1 - p1Estimate) / n1 + p2Estimate * (1 - p2Estimate) / n2);
}

}  // namespace Probability
