#include "two_means_z_statistic.h"

#include <cmath>

#include "probability/app.h"

namespace Probability {

void TwoMeansZStatistic::computeTest() {
  float deltaMean = m_hypothesisParams.firstParam();

  m_z = _z(deltaMean, x1(), n1(), sigma1(), x2(), n2(), sigma2());
  char op = static_cast<char>(m_hypothesisParams.op());
  m_pValue = _pVal(m_z, op);
}

void TwoMeansZStatistic::computeInterval() {
  m_pEstimate = _xEstimate(x1(), x2());
  m_zCritical = _zCritical(threshold());
  m_SE = _SE(sigma1(), n1(), sigma2(), n2());
  m_ME = _ME(m_zCritical, m_SE);
}

const ParameterRepr * TwoMeansZStatistic::paramReprAtIndex(int i) const {
  constexpr static ParameterRepr params[k_numberOfParams] = {
      {I18n::Message::Mean1Symbol, I18n::Message::Sample1Mean},
      {I18n::Message::N1, I18n::Message::Sample1Size},
      {I18n::Message::s1, I18n::Message::STD1},
      {I18n::Message::Mean2Symbol, I18n::Message::Sample2Mean},
      {I18n::Message::N2, I18n::Message::Sample1Size},
      {I18n::Message::s2, I18n::Message::STD2}};
  return &(params[i]);
}

float TwoMeansZStatistic::_xEstimate(float meanSample1, float meanSample2) {
  return meanSample1 - meanSample2;
}

float TwoMeansZStatistic::_z(float deltaMean, float meanSample1, float n1, float sigma1,
                             float meanSample2, float n2, float sigma2) {
  return ((meanSample1 - meanSample2) - (deltaMean)) / _SE(sigma1, n1, sigma2, n2);
}

float TwoMeansZStatistic::_SE(float sigma1, int n1, float sigma2, int n2) {
  return sqrt((sigma1 * sigma1 / n1 + sigma2 * sigma2 / n2));
}

}  // namespace Probability
