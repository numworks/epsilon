#ifndef PROBABILITE_EXPONENTIAL_DISTRIBUTION_H
#define PROBABILITE_EXPONENTIAL_DISTRIBUTION_H

#include <assert.h>
#include "one_parameter_distribution.h"
#include <poincare/layout_helper.h>

namespace Distributions {

class ExponentialDistribution final : public OneParameterDistribution {
public:
  ExponentialDistribution() : OneParameterDistribution(Poincare::Distribution::Type::Exponential, 1.0) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::ExponentialDistribution; }
  const char * parameterNameAtIndex(int index) const override { return "λ"; }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedParameterAtIndex(double x, int index) const override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double p) const override;
private:
  Shared::ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return Shared::ParameterRepresentation{Poincare::LayoutHelper::String(parameterNameAtIndex(0)), I18n::Message::LambdaExponentialDefinition};
  }
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
