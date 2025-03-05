#ifndef PROBABILITE_BINOMIAL_DISTRIBUTION_H
#define PROBABILITE_BINOMIAL_DISTRIBUTION_H

#include "omg/unreachable.h"
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
  I18n::Message messageForParameterAtIndex(int index) const override {
    switch (index) {
      case ParamsOrder::N:
        return I18n::Message::RepetitionNumber;
      case ParamsOrder::P:
        return I18n::Message::SuccessProbability;
      default:
        OMG::unreachable();
    }
  }
  float privateComputeXMin() const override;
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
