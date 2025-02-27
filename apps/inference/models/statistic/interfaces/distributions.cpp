#include "distributions.h"

#include <inference/models/statistic/statistic.h>
#include <inference/models/statistic/test.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

namespace Inference {

float Distribution::canonicalDensityFunction(float x,
                                             double degreesOfFreedom) const {
  return distribution()->evaluateAtAbscissa(
      x, constParametersArray(&degreesOfFreedom));
}

double Distribution::cumulativeNormalizedDistributionFunction(
    double x, double degreesOfFreedom) const {
  return distribution()->cumulativeDistributiveFunctionAtAbscissa(
      x, constParametersArray(&degreesOfFreedom));
}

double Distribution::cumulativeNormalizedInverseDistributionFunction(
    double proba, double degreesOfFreedom) const {
  return distribution()->cumulativeDistributiveInverseForProbability(
      proba, constParametersArray(&degreesOfFreedom));
}

/* Distribution t */

float DistributionT::yMax(double degreesOfFreedom) const {
  return (1 + Shared::Inference::k_displayTopMarginRatio) *
         canonicalDensityFunction(0, degreesOfFreedom);
}

/* Distribution z */

float DistributionZ::yMax(double degreesOfFreedom) const {
  return (1 + Shared::Inference::k_displayTopMarginRatio) *
         canonicalDensityFunction(0, degreesOfFreedom);
}

/* Distribution chi 2 */

Poincare::Layout DistributionChi2::criticalValueSymbolLayout() const {
  return "χ"_l ^ KSuperscriptL("2"_l);
}

float DistributionChi2::xMin(double degreesOfFreedom) const {
  return -Shared::Inference::k_displayLeftMarginRatio * xMax(degreesOfFreedom);
}

float DistributionChi2::xMax(double degreesOfFreedom) const {
  return (1 + Shared::Inference::k_displayRightMarginRatio) *
         (degreesOfFreedom +
          Test::k_displayWidthToSTDRatio * std::sqrt(degreesOfFreedom));
}

float DistributionChi2::yMax(double degreesOfFreedom) const {
  float max =
      degreesOfFreedom <= 2.0
          ? 0.5
          : canonicalDensityFunction(degreesOfFreedom - 1, degreesOfFreedom) *
                1.2;
  return (1. + Shared::Inference::k_displayTopMarginRatio) * max;
}

}  // namespace Inference
