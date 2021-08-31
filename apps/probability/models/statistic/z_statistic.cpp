#include "z_statistic.h"

#include <poincare/normal_distribution.h>

namespace Probability {

float ZStatistic::canonicalDensityFunction(float x) const {
  return Poincare::NormalDistribution::EvaluateAtAbscissa<float>(x, 0, 1);
}

float ZStatistic::cumulativeNormalizedDistributionFunction(float x) const {
  return Poincare::NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(x, 0, 1);
}

float ZStatistic::cumulativeNormalizedInverseDistributionFunction(float proba) const {
  return Poincare::NormalDistribution::CumulativeDistributiveInverseForProbability<float>(proba, 0, 1);
}

float ZStatistic::xMin() const {
  return -k_displayWidthToSTDRatio;
}

float ZStatistic::xMax() const {
  return k_displayWidthToSTDRatio;
}

float ZStatistic::yMax() const {
  return (1 + k_displayTopMarginRatio) * canonicalDensityFunction(0);
}

}  // namespace Probability
