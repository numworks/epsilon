#include "fisher_distribution.h"

#include <float.h>
#include <poincare/layout.h>

using namespace Shared;

namespace Distributions {

bool FisherDistribution::authorizedParameterAtIndex(double x, int index) const {
  assert(index == 0 || index == 1);
  return Distribution::authorizedParameterAtIndex(x, index) && x > DBL_MIN &&
         x <= k_maxParameter;
}

float FisherDistribution::privateComputeXMax() const {
  return Poincare::Distribution::ComputeXMax(m_distribution,
                                             constParametersArray());
}

}  // namespace Distributions
