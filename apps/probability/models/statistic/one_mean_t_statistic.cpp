#include "one_mean_t_statistic.h"

#include <cmath>

#include "probability/app.h"

namespace Probability {

void OneMeanTStatistic::computeTest() {
  Data::Data * data = App::app()->snapshot()->data();
  float mean = data->hypothesisParams()->firstParam();
  float meanSample = data->testData()->inputParameters()->paramAtIndex(0);
  float s = data->testData()->inputParameters()->paramAtIndex(1);
  int n = data->testData()->inputParameters()->paramAtIndex(2);

  m_degreesOfFreedom = _degreesOfFreedom(n);
  m_z = _t(mean, meanSample, s, n);
  m_pValue = _pVal(m_degreesOfFreedom, m_z);
}

void OneMeanTStatistic::computeInterval() {
  Data::Data * data = App::app()->snapshot()->data();
  float meanSample = data->testData()->inputParameters()->paramAtIndex(0);
  float s = data->testData()->inputParameters()->paramAtIndex(1);
  int n = data->testData()->inputParameters()->paramAtIndex(2);
  float confidenceLevel = data->testData()->inputParameters()->threshold();

  m_pEstimate = meanSample;
  m_degreesOfFreedom = _degreesOfFreedom(n);
  m_zCritical = _tCritical(m_degreesOfFreedom, confidenceLevel);
  m_SE = _SE(s, n);
  m_ME = _ME(m_zCritical, m_SE);
}

float OneMeanTStatistic::_degreesOfFreedom(int n) {
  return n - 1;
}

float OneMeanTStatistic::_t(float mean, float meanSample, float s, float n) {
  return (meanSample - mean) / (s / sqrt(n));
}

float OneMeanTStatistic::_SE(float s, float n) {
  return s / sqrt(n);
}

}  // namespace Probability
