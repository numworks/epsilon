#include "normal_distribution.h"

#include <float.h>
#include <poincare/layout.h>

#include <cmath>

#include "distributions/models/distribution/distribution.h"

using namespace Shared;

namespace Distributions {

bool NormalDistribution::authorizedParameterAtIndex(double x, int index) const {
  return Distribution::authorizedParameterAtIndex(x, index) &&
         (index == 0 || std::isnan(x) ||
          (x > DBL_MIN && std::fabs(m_parameters[0] / x) <= k_maxRatioMuSigma));
}

void NormalDistribution::setParameterAtIndex(double f, int index) {
  setParameterAtIndexWithoutComputingCurveViewRange(f, index);
  if (index == 0 &&
      std::fabs(m_parameters[0] / m_parameters[1]) > k_maxRatioMuSigma) {
    m_parameters[1] = std::fabs(m_parameters[0]) / k_maxRatioMuSigma;
  }
  computeCurveViewRange();
}

float NormalDistribution::privateComputeXMin() const {
  return Poincare::Distribution::ComputeXMin(m_distribution,
                                             constParametersArray());
}

float NormalDistribution::privateComputeXMax() const {
  return Poincare::Distribution::ComputeXMax(m_distribution,
                                             constParametersArray());
}

}  // namespace Distributions
