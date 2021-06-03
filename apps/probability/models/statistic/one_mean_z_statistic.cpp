#include "one_mean_z_statistic.h"

#include <cmath>

#include "probability/app.h"

namespace Probability {

void OneMeanZStatistic::computeTest() {
  Data::HypothesisParams * hypothesis = App::app()->snapshot()->data()->hypothesisParams();
  InputParameters * params = App::app()->snapshot()->data()->testData()->inputParameters();
  float mean = hypothesis->firstParam();
  float meanSample = params->paramAtIndex(0);
  float n = params->paramAtIndex(1);
  float sigma = params->paramAtIndex(2);

  m_z = _z(mean, meanSample, n, sigma);
  char op = static_cast<char>(hypothesis->op());
  m_pValue = _pVal(m_z, op);
}

void OneMeanZStatistic::computeInterval() {
  InputParameters * params = App::app()->snapshot()->data()->testData()->inputParameters();
  float meanSample = params->paramAtIndex(0);
  float n = params->paramAtIndex(1);
  float sigma = params->paramAtIndex(2);
  float confidenceLevel = params->paramAtIndex(3);

  m_pEstimate = meanSample;
  m_zCritical = _zCritical(confidenceLevel);
  m_SE = _SE(sigma, n);
  m_ME = _ME(m_zCritical, m_SE);
}

float OneMeanZStatistic::_z(float mean, float meanSample, float n, float sigma) {
  return (meanSample - mean) / (sigma / sqrt(n));
}

float OneMeanZStatistic::_SE(float sigma, int n) {
  return sigma / sqrt(n);
}

}  // namespace Probability
