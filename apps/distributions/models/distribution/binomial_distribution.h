#ifndef PROBABILITE_BINOMIAL_DISTRIBUTION_H
#define PROBABILITE_BINOMIAL_DISTRIBUTION_H

#include "two_parameters_distribution.h"

namespace Distributions {

class BinomialDistribution final : public TwoParametersDistribution {
 public:
  BinomialDistribution()
      : TwoParametersDistribution(Poincare::Distribution::Type::Binomial) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::BinomialDistribution;
  }
  double rightIntegralInverseForProbability(double p) const override;

 protected:
  enum ParamsOrder { N, P };
  constexpr static double k_defaultN = 20.0;
  constexpr static double k_defaultP = 0.5;
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override;
  float privateComputeXMin() const override;
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
