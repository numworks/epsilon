#include "t_statistic.h"

#include "probability/models/student_law.h"

namespace Probability {

using namespace Poincare;

float TStatistic::normalizedDensityFunction(float x) const {
  assert(m_degreesOfFreedom > 0);
  return StudentLaw::EvaluateAtAbscissa<float>(x, m_degreesOfFreedom);
}

float TStatistic::cumulativeNormalizedDistributionFunction(float x) const {
  assert(m_degreesOfFreedom > 0);
  return StudentLaw::CumulativeDistributiveFunctionAtAbscissa(x, m_degreesOfFreedom);
}

float TStatistic::cumulativeNormalizedInverseDistributionFunction(float proba) const {
  assert(m_degreesOfFreedom > 0);
  return StudentLaw::CumulativeDistributiveInverseForProbability(proba, m_degreesOfFreedom);
}

float TStatistic::xMin() const {
  return -k_displayWidthToSTDRatio;
}

float TStatistic::xMax() const {
  return k_displayWidthToSTDRatio;
}

float TStatistic::yMax() const {
  return (1 + k_displayTopMarginRatio) * normalizedDensityFunction(0);
}

}  // namespace Probability
