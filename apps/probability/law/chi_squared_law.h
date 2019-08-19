#ifndef PROBABILITY_CHI_SQUARED_LAW_H
#define PROBABILITY_CHI_SQUARED_LAW_H

#include "one_parameter_law.h"
#include <float.h>

namespace Probability {

class ChiSquaredLaw : public OneParameterLaw {
public:
  static constexpr int k_maxRegularizedGammaIterations = 1000;
  static constexpr double k_regularizedGammaPrecision = DBL_EPSILON;

  ChiSquaredLaw() : OneParameterLaw(1.0f) {}
  I18n::Message title() override { return I18n::Message::ChiSquaredLaw; }
  Type type() const override { return Type::ChiSquared; }
  bool isContinuous() const override { return true; }
  float xMin() const override;
  float xMax() const override;
  float yMax() const override;
  I18n::Message parameterNameAtIndex(int index) override {
    assert(index == 0);
    return I18n::Message::K;
  }
  I18n::Message parameterDefinitionAtIndex(int index) override {
    assert(index == 0);
    return I18n::Message::DegreesOfFreedomDefinition;
  }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedValueAtIndex(float x, int index) const override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * probability) override;
private:
  float coefficient() const;
};

}

#endif
