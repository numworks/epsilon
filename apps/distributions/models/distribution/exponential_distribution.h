#ifndef PROBABILITE_EXPONENTIAL_DISTRIBUTION_H
#define PROBABILITE_EXPONENTIAL_DISTRIBUTION_H

#include <assert.h>
#include <poincare/layout.h>

#include "one_parameter_distribution.h"

namespace Distributions {

class ExponentialDistribution final : public OneParameterDistribution {
 public:
  ExponentialDistribution()
      : OneParameterDistribution(Poincare::Distribution::Type::Exponential) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::ExponentialDistribution;
  }
  bool authorizedParameterAtIndex(double x, int index) const override;

 private:
  constexpr static double k_maxLambda = 7500.0;
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return Shared::ParameterRepresentation{
        Poincare::Layout::String(parameterNameAtIndex(0)),
        I18n::Message::LambdaExponentialDefinition};
  }
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
