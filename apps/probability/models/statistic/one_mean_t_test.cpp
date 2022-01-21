#include "one_mean_t_test.h"
#include "one_mean_z_test.h"

namespace Probability {

void OneMeanTTest::initializeDistribution(DistributionType distribution) {
  this->~OneMeanTTest();
  switch (distribution) {
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
}

}
