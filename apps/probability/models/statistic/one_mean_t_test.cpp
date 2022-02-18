#include "one_mean_t_test.h"
#include "one_mean_z_test.h"

namespace Probability {

bool OneMeanTTest::initializeDistribution(DistributionType distributionType) {
  if (Statistic::initializeDistribution(distributionType)) {
    return false;
  }
  this->~OneMeanTTest();
  switch (distributionType) {
    case DistributionType::T:
      new (this) OneMeanTTest();
      break;
    case DistributionType::Z:
      new (this) OneMeanZTest();
      break;
    default:
      assert(false);
      break;
  }
  initParameters();
  return true;
}

}
