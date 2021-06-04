#include "two_means_t_statistic.h"

#include <cmath>

#include "probability/app.h"

namespace Probability {

void TwoMeansTStatistic::computeTest() {
  float deltaMean = App::app()->snapshot()->data()->hypothesisParams()->firstParam();
  InputParameters * params = App::app()->snapshot()->data()->testData()->inputParameters();
  float meanSample1 = params->paramAtIndex(0);
  float n1 = params->paramAtIndex(1);
  float s1 = params->paramAtIndex(2);
  float meanSample2 = params->paramAtIndex(3);
  float n2 = params->paramAtIndex(4);
  float s2 = params->paramAtIndex(5);
  float significanceLevel = params->threshold();

  m_z = _t(deltaMean, meanSample1, n1, s1, meanSample2, n2, s2);
  char op = static_cast<char>(App::app()->snapshot()->data()->hypothesisParams()->op());
  m_pValue = _pVal(m_z, op);
}

void TwoMeansTStatistic::computeInterval() {
  InputParameters * params = App::app()->snapshot()->data()->testData()->inputParameters();
  float meanSample1 = params->paramAtIndex(0);
  float n1 = params->paramAtIndex(1);
  float s1 = params->paramAtIndex(2);
  float meanSample2 = params->paramAtIndex(3);
  float n2 = params->paramAtIndex(4);
  float s2 = params->paramAtIndex(5);
  float confidenceLevel = params->threshold();

  m_degreesOfFreedom = _degreeOfFreedom(s1, n1, s2, n2);
  m_pEstimate = _xEstimate(meanSample1, meanSample2);
  m_zCritical = _tCritical(m_degreesOfFreedom, confidenceLevel);
  m_SE = _SE(s1, n1, s2, n2);
  m_ME = _ME(m_SE, m_zCritical);
}

float TwoMeansTStatistic::_xEstimate(float meanSample1, float meanSample2) {
  return meanSample1 - meanSample2;
}

float TwoMeansTStatistic::_t(float deltaMean, float meanSample1, float n1, float s1, float meanSample2, float n2,
                             float s2) {
  return ((meanSample1 - meanSample2) - (deltaMean)) /
         _SE(s1, n1, s2, n2);
}

float TwoMeansTStatistic::_degreeOfFreedom(float s1, int n1, float s2, int n2) {
  float v1 = pow(s1, 2) / n1;
  float v2 = pow(s2, 2) / n2;
  return pow(v1 + v2, 2) / (pow(v1, 2) / (n1 - 1) + pow(v2, 2) / (n2 - 1));
}

float TwoMeansTStatistic::_SE(float s1, int n1, float s2, int n2) {
  return sqrt((s1 * s1 / n1 + s2 * s2 / n2));
}

} // namespace Probability
