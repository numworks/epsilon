#include "chi_squared_distribution.h"

#include <cmath>

namespace Distributions {

bool ChiSquaredDistribution::authorizedParameterAtIndex(double x,
                                                        int index) const {
  return x <= k_maxK && std::floor(x) == x &&
         Distribution::authorizedParameterAtIndex(x, index);
}

}  // namespace Distributions
