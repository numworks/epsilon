#include "two_means_z_statistic.h"

#include <cmath>

#include "probability/app.h"

namespace Probability {

void TwoMeansZStatistic::computeTest() {
  float deltaMean = App::app()->snapshot()->data()->hypothesisParams()->firstParam();
  InputParameters * params = App::app()->snapshot()->data()->testData()->inputParameters();
  float meanSample1 = params->paramAtIndex(0);
  float n1 = params->paramAtIndex(1);
  float sigma1 = params->paramAtIndex(2);
  float meanSample2 = params->paramAtIndex(3);
  float n2 = params->paramAtIndex(4);
  float sigma2 = params->paramAtIndex(5);
  float significanceLevel = params->threshold();

  m_z = _z(deltaMean, meanSample1, n1, sigma1, meanSample2, n2, sigma2);
  char op = static_cast<char>(App::app()->snapshot()->data()->hypothesisParams()->op());
  m_pValue = _pVal(m_z, op);
}

void TwoMeansZStatistic::computeInterval() {
  InputParameters * params = App::app()->snapshot()->data()->testData()->inputParameters();
  float meanSample1 = params->paramAtIndex(0);
  float n1 = params->paramAtIndex(1);
  float sigma1 = params->paramAtIndex(2);
  float meanSample2 = params->paramAtIndex(3);
  float n2 = params->paramAtIndex(4);
  float sigma2 = params->paramAtIndex(5);
  float confidenceLevel = params->threshold();

  m_pEstimate = _xEstimate(meanSample1, meanSample2);
  m_zCritical = _zCritical(confidenceLevel);
  m_SE = _SE(sigma1, n1, sigma2, n2);
  m_ME = _ME(m_zCritical, m_SE);
}

float TwoMeansZStatistic::_xEstimate(float meanSample1, float meanSample2) {
  return meanSample1 - meanSample2;
}

float TwoMeansZStatistic::_z(float deltaMean, float meanSample1, float n1, float sigma1,
                             float meanSample2, float n2, float sigma2) {
  return ((meanSample1 - meanSample2) - (deltaMean)) /
         _SE(sigma1, n1, sigma2, n2);
}

float TwoMeansZStatistic::_SE(float sigma1, int n1, float sigma2, int n2) {
  return sqrt((sigma1 * sigma1 / n1 + sigma2 * sigma2 / n2));
}

}  // namespace Probability
