#include "poisson_distribution.h"

#include <assert.h>

namespace Distributions {

bool PoissonDistribution::authorizedParameterAtIndex(double x,
                                                     int index) const {
  return Distribution::authorizedParameterAtIndex(x, index) && x > 0.0 &&
         x <= 999.0;
}

}  // namespace Distributions
