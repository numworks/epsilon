#ifndef PROBABILITE_EXPONENTIAL_LAW_H
#define PROBABILITE_EXPONENTIAL_LAW_H

#include "one_parameter_law.h"
#include <assert.h>

namespace Probability {

class ExponentialLaw final : public OneParameterLaw {
public:
  ExponentialLaw() : OneParameterLaw(1.0f) {}
  I18n::Message title() override { return I18n::Message::ExponentialLaw; }
  Type type() const override { return Type::Exponential; }
  bool isContinuous() const override { return true; }
  float xMin() override;
  float xMax() override;
  float yMax() override;
  I18n::Message parameterNameAtIndex(int index) override {
    assert(index == 0);
    return I18n::Message::Lambda;
  }
  I18n::Message parameterDefinitionAtIndex(int index) override {
    assert(index == 0);
    return I18n::Message::LambdaExponentialDefinition;
  }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedValueAtIndex(float x, int index) const override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * probability) override;
};

}

#endif
