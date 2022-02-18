#include "one_mean_t_interval.h"
#include "one_mean_z_interval.h"

namespace Probability {

bool OneMeanTInterval::initializeDistribution(DistributionType distributionType) {
  if (Statistic::initializeDistribution(distributionType)) {
    return false;
  }
  this->~OneMeanTInterval();
  switch (distributionType) {
    case DistributionType::T:
      new (this) OneMeanTInterval();
      break;
    case DistributionType::Z:
      new (this) OneMeanZInterval();
      break;
    default:
      assert(false);
      break;
  }
  initParameters();
  return true;
}

}
