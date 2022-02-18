#include "pooled_two_means_t_test.h"
#include "two_means_t_test.h"
#include "two_means_z_test.h"

namespace Probability {

bool TwoMeansTTest::initializeDistribution(DistributionType distributionType) {
  if (Statistic::initializeDistribution(distributionType)) {
    return false;
  }
  this->~TwoMeansTTest();
  switch (distributionType) {
    case DistributionType::T:
      new (this) TwoMeansTTest();
      break;
    case DistributionType::TPooled:
      new (this) PooledTwoMeansTTest();
      break;
    case DistributionType::Z:
      new (this) TwoMeansZTest();
      break;
    default:
      assert(false);
      break;
  }
  initParameters();
  return true;
}

}
