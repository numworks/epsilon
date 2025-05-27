#include "exponential_distribution.h"

#include "distributions/models/distribution/distribution.h"

namespace Distributions {

bool ExponentialDistribution::authorizedParameterAtIndex(double x,
                                                         int index) const {
  return Distribution::authorizedParameterAtIndex(x, index) && x <= k_maxLambda;
}

}  // namespace Distributions
