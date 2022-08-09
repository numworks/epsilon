#ifndef INFERENCE_MODELS_STATISTIC_INTERFACES_DISTRIBUTIONS_H
#define INFERENCE_MODELS_STATISTIC_INTERFACES_DISTRIBUTIONS_H

#include <inference/models/statistic/statistic.h>
#include <inference/models/statistic/test.h>
#include <poincare/layout_helper.h>

namespace Inference {

class DistributionInterface {
public:
  static float XMin() { return -Test::k_displayWidthToSTDRatio; }
  static float XMax() { return Test::k_displayWidthToSTDRatio; }
};

class DistributionT : public DistributionInterface {
public:
  static Poincare::Layout TestCriticalValueSymbol() {
    return Poincare::LayoutHelper::String("t", -1);
  }
  static I18n::Message GraphTitleFormat() { return I18n::Message::StatisticGraphControllerTestTitleFormatTTest; }
  static float CanonicalDensityFunction(float x, double degreesOfFreedom);
  static double CumulativeNormalizedDistributionFunction(double x, double degreesOfFreedom);
  static double CumulativeNormalizedInverseDistributionFunction(double proba, double degreesOfFreedom);

  static float YMax(double degreesOfFreedom);
};

class DistributionZ : public DistributionInterface {
public:
  static Poincare::Layout TestCriticalValueSymbol() {
    return Poincare::LayoutHelper::String("z", -1);
  }
  static I18n::Message GraphTitleFormat() { return I18n::Message::StatisticGraphControllerTestTitleFormatZtest; }
  static float CanonicalDensityFunction(float x, double degreesOfFreedom);
  static double CumulativeNormalizedDistributionFunction(double x, double degreesOfFreedom);
  static double CumulativeNormalizedInverseDistributionFunction(double proba, double degreesOfFreedom);

  static float YMax(double degreesOfFreedom);
};

class DistributionChi2 : public DistributionInterface {
public:
  static Poincare::Layout TestCriticalValueSymbol();
  static float CanonicalDensityFunction(float x, double degreesOfFreedom);
  static double CumulativeNormalizedDistributionFunction(double x, double degreesOfFreedom);
  static double CumulativeNormalizedInverseDistributionFunction(double proba, double degreesOfFreedom);

  static float XMin(double degreesOfFreedom);
  static float XMax(double degreesOfFreedom);
  static float YMax(double degreesOfFreedom);
};

}

#endif
