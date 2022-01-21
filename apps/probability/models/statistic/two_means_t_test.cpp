#include "pooled_two_means_t_test.h"
#include "two_means_t_test.h"
#include "two_means_z_test.h"

namespace Probability {

void TwoMeansTTest::initializeDistribution(DistributionType distribution) {
  this->~TwoMeansTTest();
  switch (distribution) {
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
}

}
