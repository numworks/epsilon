#include "t_statistic.h"
#include <poincare/student_distribution.h>

namespace Probability {

using namespace Poincare;

float TStatistic::canonicalDensityFunction(float x) const {
  assert(m_degreesOfFreedom > 0);
  return StudentDistribution::EvaluateAtAbscissa<float>(x, m_degreesOfFreedom);
}

double TStatistic::cumulativeNormalizedDistributionFunction(double x) const {
  assert(m_degreesOfFreedom > 0);
  return StudentDistribution::CumulativeDistributiveFunctionAtAbscissa(x, m_degreesOfFreedom);
}

double TStatistic::cumulativeNormalizedInverseDistributionFunction(double proba) const {
  assert(m_degreesOfFreedom > 0);
  return StudentDistribution::CumulativeDistributiveInverseForProbability(proba, m_degreesOfFreedom);
}

float TStatistic::xMin() const {
  return -k_displayWidthToSTDRatio;
}

float TStatistic::xMax() const {
  return k_displayWidthToSTDRatio;
}

float TStatistic::yMax() const {
  return (1 + k_displayTopMarginRatio) * canonicalDensityFunction(0);
}

}  // namespace Probability
