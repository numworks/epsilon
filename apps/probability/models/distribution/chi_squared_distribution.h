#ifndef PROBABILITY_CHI_SQUARED_DISTRIBUTION_H
#define PROBABILITY_CHI_SQUARED_DISTRIBUTION_H

#include "one_parameter_distribution.h"
#include <float.h>

namespace Probability {

class ChiSquaredDistribution : public OneParameterDistribution {
public:
  ChiSquaredDistribution() : OneParameterDistribution(1.0) { computeCurveViewRange(); }
  I18n::Message title() override { return I18n::Message::ChiSquaredDistribution; }
  Type type() const override { return Type::ChiSquared; }
  bool isContinuous() const override { return true; }
  bool isSymetrical() const override { return false; }
  I18n::Message parameterNameAtIndex(int index) override {
    assert(index == 0);
    return I18n::Message::K;
  }
  I18n::Message parameterDefinitionAtIndex(int index) override {
    assert(index == 0);
    return I18n::Message::DegreesOfFreedomDefinition;
  }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedValueAtIndex(double x, int index) const override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * probability) override;
private:
  static constexpr double k_maxK = 31500.0;
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
