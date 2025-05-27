#ifndef PROBABILITE_POISSON_DISTRIBUTION_H
#define PROBABILITE_POISSON_DISTRIBUTION_H

#include <apps/i18n.h>
#include <poincare/layout.h>

#include "one_parameter_distribution.h"

namespace Distributions {

class PoissonDistribution final : public OneParameterDistribution {
 public:
  PoissonDistribution()
      : OneParameterDistribution(Poincare::Distribution::Type::Poisson) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::PoissonDistribution;
  }
  bool authorizedParameterAtIndex(double x, int index) const override;

 private:
  I18n::Message messageForParameterAtIndex(int index) const override {
    return I18n::Message::LambdaPoissonDefinition;
  }
};

}  // namespace Distributions

#endif
