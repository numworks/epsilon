#include "two_proportions_statistic.h"

#include <poincare/normal_distribution.h>

#include <cmath>

#include "probability/app.h"
#include "probability/models/input_parameters.h"

namespace Probability {

void TwoProportionsStatistic::computeTest() {
  Data::Data * data = App::app()->snapshot()->data();
  float deltaP0 = data->hypothesisParams()->firstParam();
  float x1 = data->testData()->inputParameters()->paramAtIndex(0);
  float n1 = data->testData()->inputParameters()->paramAtIndex(1);
  float x2 = data->testData()->inputParameters()->paramAtIndex(2);
  float n2 = data->testData()->inputParameters()->paramAtIndex(3);

  m_z = _z(deltaP0, x1, n1, x2, n2);
  char op = static_cast<char>(data->hypothesisParams()->op());
  m_pValue = _pVal(m_z, op);
}

void TwoProportionsStatistic::computeInterval() {
  InputParameters * params = App::app()->snapshot()->data()->testData()->inputParameters();
  float x1 = params->paramAtIndex(0);
  int n1 = params->paramAtIndex(1);
  float x2 = params->paramAtIndex(2);
  int n2 = params->paramAtIndex(3);
  float confidenceLevel = params->threshold();

  m_pEstimate = _pEstimate(x1, n1, x2, n2);
  m_zCritical = _zCritical(confidenceLevel);
  m_SE = _SE(x1, n1, x2, n2);
  m_ME = _ME(m_zCritical, m_SE);
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
