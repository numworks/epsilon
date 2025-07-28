#include "student_distribution.h"

#include <poincare/statistics/distribution.h>

namespace Distributions {

bool StudentDistribution::authorizedParameterAtIndex(double x,
                                                     int index) const {
  // We cannot draw the curve for x > 200 (coefficient() is too small)
  return Distribution::authorizedParameterAtIndex(x, index) &&
         x >= DBL_EPSILON && x <= 200.0;
}

}  // namespace Distributions
