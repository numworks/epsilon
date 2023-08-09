#include "statistic.h"

#include "one_mean_t_interval.h"
#include "one_mean_t_test.h"

namespace Inference {

double Statistic::parameterAtIndex(int i) const {
  assert(i <= indexOfThreshold() &&
         indexOfThreshold() == numberOfStatisticParameters());
  return i == indexOfThreshold()
             ? m_threshold
             : const_cast<Statistic *>(this)->parametersArray()[i];
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
  DistributionType type = distributionType();
  switch (type) {
    case DistributionType::T:
    case DistributionType::TPooled:
      return DistributionT::CumulativeNormalizedDistributionFunction(
          x, m_degreesOfFreedom);
    case DistributionType::Z:
      return DistributionZ::CumulativeNormalizedDistributionFunction(
          x, m_degreesOfFreedom);
    default:
      assert(type == DistributionType::Chi2);
      return DistributionChi2::CumulativeNormalizedDistributionFunction(
          x, m_degreesOfFreedom);
  }
}

double Statistic::cumulativeDistributiveInverseForProbability(
    double probability) const {
  DistributionType type = distributionType();
  switch (type) {
    case DistributionType::T:
    case DistributionType::TPooled:
      return DistributionT::CumulativeNormalizedInverseDistributionFunction(
          probability, m_degreesOfFreedom);
    case DistributionType::Z:
      return DistributionZ::CumulativeNormalizedInverseDistributionFunction(
          probability, m_degreesOfFreedom);
    default:
      assert(type == DistributionType::Chi2);
      return DistributionChi2::CumulativeNormalizedInverseDistributionFunction(
          probability, m_degreesOfFreedom);
  }
}

bool Statistic::Initialize(Statistic *statistic, SubApp subApp) {
  if (statistic->subApp() == subApp) {
    return false;
  }
  statistic->~Statistic();
  Statistic *s = nullptr;
  switch (subApp) {
    case SubApp::Test:
      s = new (statistic) OneMeanTTest();
      break;
    case SubApp::Interval:
      s = new (statistic) OneMeanTInterval();
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
  DistributionType type = distributionType();
  switch (type) {
    case DistributionType::T:
    case DistributionType::TPooled:
      return DistributionT::CriticalValueSymbolLayout();
    case DistributionType::Z:
      return DistributionZ::CriticalValueSymbolLayout();
    default:
      assert(type == DistributionType::Chi2);
      return DistributionChi2::CriticalValueSymbolLayout();
  }
}

float Statistic::computeYMax() const {
  DistributionType type = distributionType();
  switch (type) {
    case DistributionType::T:
    case DistributionType::TPooled:
      return DistributionT::YMax(m_degreesOfFreedom);
    case DistributionType::Z:
      return DistributionZ::YMax(m_degreesOfFreedom);
    default:
      assert(type == DistributionType::Chi2);
      return DistributionChi2::YMax(m_degreesOfFreedom);
  }
}

float Statistic::canonicalDensityFunction(float x) const {
  DistributionType type = distributionType();
  switch (type) {
    case DistributionType::T:
    case DistributionType::TPooled:
      return DistributionT::CanonicalDensityFunction(x, m_degreesOfFreedom);
    case DistributionType::Z:
      return DistributionZ::CanonicalDensityFunction(x, m_degreesOfFreedom);
    default:
      assert(type == DistributionType::Chi2);
      return DistributionChi2::CanonicalDensityFunction(x, m_degreesOfFreedom);
  }
}

}  // namespace Inference
