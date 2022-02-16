#ifndef PROBABILITE_EXPONENTIAL_DISTRIBUTION_H
#define PROBABILITE_EXPONENTIAL_DISTRIBUTION_H

#include "one_parameter_distribution.h"
#include <assert.h>

namespace Probability {

class ExponentialDistribution final : public OneParameterDistribution {
public:
  ExponentialDistribution() : OneParameterDistribution(1.0) { computeCurveViewRange(); }
  I18n::Message title() override { return I18n::Message::ExponentialDistribution; }
  Type type() const override { return Type::Exponential; }
  bool isContinuous() const override { return true; }
  bool isSymmetrical() const override { return false; }
  I18n::Message parameterNameAtIndex(int index) override {
    assert(index == 0);
    return I18n::Message::Lambda;
  }
  I18n::Message parameterDefinitionAtIndex(int index) override {
    assert(index == 0);
    return I18n::Message::LambdaExponentialDefinition;
  }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedValueAtIndex(double x, int index) const override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * probability) override;
private:
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
