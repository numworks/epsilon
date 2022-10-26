#include "distributions.h"
#include <poincare/test/helper.h>
#include <poincare/chi2_distribution.h>
#include <poincare/normal_distribution.h>
#include <poincare/student_distribution.h>
#include <poincare/code_point_layout.h>
#include <poincare/vertical_offset_layout.h>

namespace Inference {

/* Distribution t */

float DistributionT::CanonicalDensityFunction(float x, double degreesOfFreedom) {
  assert(degreesOfFreedom > 0);
  return Poincare::StudentDistribution::EvaluateAtAbscissa<float>(x, degreesOfFreedom);
}

double DistributionT::CumulativeNormalizedDistributionFunction(double x, double degreesOfFreedom) {
  assert(degreesOfFreedom > 0);
  return Poincare::StudentDistribution::CumulativeDistributiveFunctionAtAbscissa(x, degreesOfFreedom);
}

double DistributionT::CumulativeNormalizedInverseDistributionFunction(double proba, double degreesOfFreedom) {
  assert(degreesOfFreedom > 0);
  return Poincare::StudentDistribution::CumulativeDistributiveInverseForProbability(proba, degreesOfFreedom);
}

float DistributionT::YMax(double degreesOfFreedom) {
  return (1 + Shared::Inference::k_displayTopMarginRatio) * CanonicalDensityFunction(0, degreesOfFreedom);
}

/* Distribution z */

float DistributionZ::CanonicalDensityFunction(float x, double degreesOfFreedom) {
  return Poincare::NormalDistribution::EvaluateAtAbscissa<float>(x, 0, 1);
}

double DistributionZ::CumulativeNormalizedDistributionFunction(double x, double degreesOfFreedom) {
  return Poincare::NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(x, 0, 1);
}

double DistributionZ::CumulativeNormalizedInverseDistributionFunction(double proba, double degreesOfFreedom) {
  return Poincare::NormalDistribution::CumulativeDistributiveInverseForProbability<double>(proba, 0, 1);
}

float DistributionZ::YMax(double degreesOfFreedom) {
  return (1 + Shared::Inference::k_displayTopMarginRatio) * CanonicalDensityFunction(0, degreesOfFreedom);
}

/* Distribution chi 2 */

Poincare::Layout DistributionChi2::TestCriticalValueSymbol() {
  return Poincare::HorizontalLayout::Builder(
      Poincare::CodePointLayout::Builder(UCodePointGreekSmallLetterChi),
      Poincare::VerticalOffsetLayout::Builder(
          Poincare::CodePointLayout::Builder('2'),
          Poincare::VerticalOffsetLayoutNode::VerticalPosition::Superscript));
}

float DistributionChi2::CanonicalDensityFunction(float x, double degreesOfFreedom) {
  assert(degreesOfFreedom > 0);
  return Poincare::Chi2Distribution::EvaluateAtAbscissa(x, static_cast<float>(degreesOfFreedom));

}

double DistributionChi2::CumulativeNormalizedDistributionFunction(double x, double degreesOfFreedom) {
  assert(degreesOfFreedom > 0);
  return Poincare::Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa(x, degreesOfFreedom);
}

double DistributionChi2::CumulativeNormalizedInverseDistributionFunction(double proba, double degreesOfFreedom) {
  assert(degreesOfFreedom > 0);
  return Poincare::Chi2Distribution::CumulativeDistributiveInverseForProbability(proba, degreesOfFreedom);
}

float DistributionChi2::XMin(double degreesOfFreedom) {
  return -Shared::Inference::k_displayLeftMarginRatio * XMax(degreesOfFreedom);
}

float DistributionChi2::XMax(double degreesOfFreedom) {
  return (1 + Shared::Inference::k_displayRightMarginRatio) * (degreesOfFreedom + Test::k_displayWidthToSTDRatio * std::sqrt(degreesOfFreedom));
}

float DistributionChi2::YMax(double degreesOfFreedom) {
  float max = degreesOfFreedom <= 2.0 ? 0.5 : CanonicalDensityFunction(degreesOfFreedom - 1, degreesOfFreedom) * 1.2;
  return (1. + Shared::Inference::k_displayTopMarginRatio) * max;
}


}
