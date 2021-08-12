#include "z_statistic.h"

#include <poincare/normal_distribution.h>

#include "probability/models/data.h"

namespace Probability {

float ZStatistic::normalizedDensityFunction(float x) const {
  return Poincare::NormalDistribution::EvaluateAtAbscissa<float>(x, 0, 1);
}

float ZStatistic::cumulativeNormalizedDistributionFunction(float x) const {
  return Poincare::NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(x, 0, 1);
}

float ZStatistic::cumulativeNormalizedInverseDistributionFunction(float proba) const {
  return Poincare::NormalDistribution::CumulativeDistributiveInverseForProbability<float>(proba, 0, 1);
}

}  // namespace Probability
