#include "distributions.h"

#include <inference/models/statistic/statistic.h>
#include <inference/models/statistic/test.h>
#include <poincare/chi2_distribution.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/normal_distribution.h>
#include <poincare/student_distribution.h>

using namespace PoincareJ;

namespace Inference {

/* Distribution t */

float DistributionT::canonicalDensityFunction(float x,
                                              double degreesOfFreedom) const {
  assert(degreesOfFreedom > 0);
  return Poincare::StudentDistribution::EvaluateAtAbscissa<float>(
      x, degreesOfFreedom);
}

double DistributionT::cumulativeNormalizedDistributionFunction(
    double x, double degreesOfFreedom) const {
  assert(degreesOfFreedom > 0);
  return Poincare::StudentDistribution::
      CumulativeDistributiveFunctionAtAbscissa(x, degreesOfFreedom);
}

double DistributionT::cumulativeNormalizedInverseDistributionFunction(
    double proba, double degreesOfFreedom) const {
  assert(degreesOfFreedom > 0);
  return Poincare::StudentDistribution::
      CumulativeDistributiveInverseForProbability(proba, degreesOfFreedom);
}

float DistributionT::yMax(double degreesOfFreedom) const {
  return (1 + Shared::Inference::k_displayTopMarginRatio) *
         canonicalDensityFunction(0, degreesOfFreedom);
}

/* Distribution z */

float DistributionZ::canonicalDensityFunction(float x,
                                              double degreesOfFreedom) const {
  return Poincare::NormalDistribution::EvaluateAtAbscissa<float>(x, 0, 1);
}

double DistributionZ::cumulativeNormalizedDistributionFunction(
    double x, double degreesOfFreedom) const {
  return Poincare::NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<
      double>(x, 0, 1);
}

double DistributionZ::cumulativeNormalizedInverseDistributionFunction(
    double proba, double degreesOfFreedom) const {
  return Poincare::NormalDistribution::
      CumulativeDistributiveInverseForProbability<double>(proba, 0, 1);
}

float DistributionZ::yMax(double degreesOfFreedom) const {
  return (1 + Shared::Inference::k_displayTopMarginRatio) *
         canonicalDensityFunction(0, degreesOfFreedom);
}

/* Distribution chi 2 */

Poincare::Layout DistributionChi2::criticalValueSymbolLayout() const {
  return "χ"_l ^ KSuperscriptL("2"_l);
}

float DistributionChi2::canonicalDensityFunction(
    float x, double degreesOfFreedom) const {
  assert(degreesOfFreedom > 0);
  return Poincare::Chi2Distribution::EvaluateAtAbscissa(
      x, static_cast<float>(degreesOfFreedom));
}

double DistributionChi2::cumulativeNormalizedDistributionFunction(
    double x, double degreesOfFreedom) const {
  assert(degreesOfFreedom > 0);
  return Poincare::Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa(
      x, degreesOfFreedom);
}

double DistributionChi2::cumulativeNormalizedInverseDistributionFunction(
    double proba, double degreesOfFreedom) const {
  assert(degreesOfFreedom > 0);
  return Poincare::Chi2Distribution::
      CumulativeDistributiveInverseForProbability(proba, degreesOfFreedom);
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
