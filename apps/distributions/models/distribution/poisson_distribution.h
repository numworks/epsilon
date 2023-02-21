#ifndef PROBABILITE_POISSON_DISTRIBUTION_H
#define PROBABILITE_POISSON_DISTRIBUTION_H

#include <poincare/layout_helper.h>

#include "one_parameter_distribution.h"

namespace Distributions {

class PoissonDistribution final : public OneParameterDistribution {
 public:
  PoissonDistribution()
      : OneParameterDistribution(Poincare::Distribution::Type::Poisson,
                                 k_defaultLambda) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::PoissonDistribution;
  }
  const char* parameterNameAtIndex(int index) const override { return "λ"; }
  bool authorizedParameterAtIndex(double x, int index) const override;
  double defaultParameterAtIndex(int index) const override {
    return k_defaultLambda;
  }

 private:
  constexpr static double k_defaultLambda = 4.0;
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return Shared::ParameterRepresentation{
        Poincare::LayoutHelper::String(parameterNameAtIndex(0)),
        I18n::Message::LambdaPoissonDefinition};
  }
  float computeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
