#include "statistic.h"

#include <apps/apps_container_helper.h>
#include <inference/app.h>

#include "one_mean_interval.h"
#include "one_mean_test.h"

namespace Inference {

const Distribution* Statistic::distribution() const {
  DistributionType type = distributionType();
  switch (type) {
    case DistributionType::T:
    case DistributionType::TPooled:
      return &DistribT;
    case DistributionType::Z:
      return &DistribZ;
    default:
      assert(type == DistributionType::Chi2);
      return &DistribChi2;
  }
}

double Statistic::parameterAtIndex(int i) const {
  assert(i <= indexOfThreshold() &&
         indexOfThreshold() == numberOfStatisticParameters());
  return i == indexOfThreshold()
             ? m_threshold
             : const_cast<Statistic*>(this)->parametersArray()[i];
}

void Statistic::setParameterAtIndex(double f, int i) {
  assert(i <= indexOfThreshold() &&
         indexOfThreshold() == numberOfStatisticParameters());
  if (i == indexOfThreshold()) {
    m_threshold = f;
  } else {
    assert(i < indexOfThreshold());
    parametersArray()[i] = f;
  }
}

double Statistic::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  return distribution()->cumulativeNormalizedDistributionFunction(
      x, m_degreesOfFreedom);
}

double Statistic::cumulativeDistributiveInverseForProbability(
    double probability) const {
  return distribution()->cumulativeNormalizedInverseDistributionFunction(
      probability, m_degreesOfFreedom);
}

bool Statistic::Initialize(Statistic* statistic, SubApp subApp) {
  if (statistic->subApp() == subApp) {
    return false;
  }
  statistic->~Statistic();
  Statistic* s = nullptr;
  switch (subApp) {
    case SubApp::Test:
      s = new (statistic)
          OneMeanTTest(AppsContainerHelper::sharedAppsContainerGlobalContext());
      break;
    case SubApp::Interval:
      s = new (statistic) OneMeanTInterval(
          AppsContainerHelper::sharedAppsContainerGlobalContext());
      break;
    default:
      assert(false);
  }
  if (s) {
    s->initParameters();
  }
  return true;
}

Poincare::Layout Statistic::criticalValueSymbolLayout() {
  return distribution()->criticalValueSymbolLayout();
}

float Statistic::computeYMax() const {
  return distribution()->yMax(m_degreesOfFreedom);
}

float Statistic::canonicalDensityFunction(float x) const {
  return distribution()->canonicalDensityFunction(x, m_degreesOfFreedom);
}

}  // namespace Inference
