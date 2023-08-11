#ifndef INFERENCE_MODELS_STATISTIC_INTERFACES_DISTRIBUTIONS_H
#define INFERENCE_MODELS_STATISTIC_INTERFACES_DISTRIBUTIONS_H

#include <inference/models/statistic/statistic.h>
#include <inference/models/statistic/test.h>
#include <poincare/layout_helper.h>

namespace Inference {

class DistributionT {
 public:
  static const char* CriticalValueSymbol() { return "t"; }
  static Poincare::Layout CriticalValueSymbolLayout() {
    return Poincare::LayoutHelper::String("t", -1);
  }
  static float CanonicalDensityFunction(float x, double degreesOfFreedom);
  static double CumulativeNormalizedDistributionFunction(
      double x, double degreesOfFreedom);
  static double CumulativeNormalizedInverseDistributionFunction(
      double proba, double degreesOfFreedom);

  static float YMax(double degreesOfFreedom);
};

class DistributionZ {
 public:
  static const char* CriticalValueSymbol() { return "z"; }
  static Poincare::Layout CriticalValueSymbolLayout() {
    return Poincare::LayoutHelper::String("z", -1);
  }
  static float CanonicalDensityFunction(float x, double degreesOfFreedom);
  static double CumulativeNormalizedDistributionFunction(
      double x, double degreesOfFreedom);
  static double CumulativeNormalizedInverseDistributionFunction(
      double proba, double degreesOfFreedom);

  static float YMax(double degreesOfFreedom);
};

class DistributionChi2 {
 public:
  static const char* CriticalValueSymbol() { return "χ2"; }
  static Poincare::Layout CriticalValueSymbolLayout();
  static float CanonicalDensityFunction(float x, double degreesOfFreedom);
  static double CumulativeNormalizedDistributionFunction(
      double x, double degreesOfFreedom);
  static double CumulativeNormalizedInverseDistributionFunction(
      double proba, double degreesOfFreedom);

  static float XMin(double degreesOfFreedom);
  static float XMax(double degreesOfFreedom);
  static float YMax(double degreesOfFreedom);
};

}  // namespace Inference

#endif
