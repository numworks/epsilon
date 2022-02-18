#include "two_means_t_interval.h"
#include "pooled_two_means_t_interval.h"
#include "two_means_z_interval.h"

namespace Probability {

bool TwoMeansTInterval::initializeDistribution(DistributionType distributionType) {
  if (Statistic::initializeDistribution(distributionType)) {
    return false;
  }
  this->~TwoMeansTInterval();
  switch (distributionType) {
    case DistributionType::T:
      new (this) TwoMeansTInterval();
      break;
    case DistributionType::TPooled:
      new (this) PooledTwoMeansTInterval();
      break;
    case DistributionType::Z:
      new (this) TwoMeansZInterval();
      break;
    default:
      assert(false);
      break;
  }
  initParameters();
  return true;
}

}
