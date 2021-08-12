#include "t_statistic.h"

#include <poincare/normal_distribution.h>

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

}  // namespace Probability
