#include "z_statistic.h"

#include <poincare/normal_distribution.h>

#include "probability/models/data.h"

namespace Probability {

float ZStatistic::_pVal(float z, char op) {
  Data::ComparisonOperator realOp = static_cast<Data::ComparisonOperator>(op);
  switch (realOp) {
    case Data::ComparisonOperator::Lower:
      return Poincare::NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(z, 0, 1);
      break;
    case Data::ComparisonOperator::Higher:
      return 1 - Poincare::NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(
                     -z, 0, 1);
    case Data::ComparisonOperator::Different:
      return 2 * Poincare::NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(
                     -z / 2, 0, 1);
      ;
  }
}


float ZStatistic::normedDensityFunction(float x) {
  return Poincare::NormalDistribution::EvaluateAtAbscissa<float>(x, 0, 1);
}

float ZStatistic::_zCritical(float confidenceLevel) {
  return Poincare::NormalDistribution::CumulativeDistributiveInverseForProbability<float>(
      confidenceLevel, 0, 1);
}

}  // namespace Probability
