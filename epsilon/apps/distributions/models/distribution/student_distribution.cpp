#include "student_distribution.h"

#include <poincare/statistics/distribution.h>

namespace Distributions {

bool StudentDistribution::authorizedParameterAtIndex(double x,
                                                     int index) const {
  return Distribution::authorizedParameterAtIndex(x, index) &&
         x >= DBL_EPSILON && x <= k_maxParameter;
}

}  // namespace Distributions
