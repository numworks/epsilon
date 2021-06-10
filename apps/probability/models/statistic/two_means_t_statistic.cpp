#include "two_means_t_statistic.h"

#include <cmath>

#include "probability/app.h"

namespace Probability {

TwoMeansTStatistic::TwoMeansTStatistic() {
  m_params[ParamsOrder::X1] = 20;
  m_params[ParamsOrder::N1] = 50;
  m_params[ParamsOrder::S1] = 2;
  m_params[ParamsOrder::X2] = 40;
  m_params[ParamsOrder::N2] = 60;
  m_params[ParamsOrder::S2] = 2;
}

void TwoMeansTStatistic::computeTest() {
  float deltaMean = m_hypothesisParams.firstParam();
  m_zAlpha = _tAlpha(m_degreesOfFreedom, m_threshold);
  m_z = _t(deltaMean, x1(), n1(), s1(), x2(), n2(), s2());
  char op = static_cast<char>(m_hypothesisParams.op());
  m_pValue = _pVal(m_z, op);
}

void TwoMeansTStatistic::computeInterval() {
  m_degreesOfFreedom = _degreeOfFreedom(s1(), n1(), s2(), n2());
  m_pEstimate = _xEstimate(x1(), x2());
  m_zCritical = _tCritical(m_degreesOfFreedom, threshold());
  m_SE = _SE(s1(), n1(), s2(), n2());
  m_ME = _ME(m_SE, m_zCritical);
}

const ParameterRepr * TwoMeansTStatistic::paramReprAtIndex(int i) const {
  constexpr static ParameterRepr params[k_numberOfParams] = {
      {I18n::Message::Mean1Symbol, I18n::Message::Sample1Mean},
      {I18n::Message::s1, I18n::Message::STD1},
      {I18n::Message::N1, I18n::Message::Sample1Size},
      {I18n::Message::Mean2Symbol, I18n::Message::Sample2Mean},
      {I18n::Message::s2, I18n::Message::STD2},
      {I18n::Message::N2, I18n::Message::Sample1Size}};
  return &params[i];
}

float TwoMeansTStatistic::_xEstimate(float meanSample1, float meanSample2) {
  return meanSample1 - meanSample2;
}

float TwoMeansTStatistic::_t(float deltaMean, float meanSample1, float n1, float s1,
                             float meanSample2, float n2, float s2) {
  return ((meanSample1 - meanSample2) - (deltaMean)) / _SE(s1, n1, s2, n2);
}

float TwoMeansTStatistic::_degreeOfFreedom(float s1, int n1, float s2, int n2) {
  float v1 = pow(s1, 2) / n1;
  float v2 = pow(s2, 2) / n2;
  return pow(v1 + v2, 2) / (pow(v1, 2) / (n1 - 1) + pow(v2, 2) / (n2 - 1));
}

float TwoMeansTStatistic::_SE(float s1, int n1, float s2, int n2) {
  return sqrt((s1 * s1 / n1 + s2 * s2 / n2));
}

}  // namespace Probability
