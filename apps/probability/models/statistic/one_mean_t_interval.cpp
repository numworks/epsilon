#include "one_mean_t_interval.h"
#include "one_mean_z_interval.h"

namespace Probability {

void OneMeanTInterval::initializeDistribution(DistributionType distribution) {
  this->~OneMeanTInterval();
  switch (distribution) {
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
}

}
