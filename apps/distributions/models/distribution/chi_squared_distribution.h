#ifndef DISTRIBUTION_CHI_SQUARED_DISTRIBUTION_H
#define DISTRIBUTION_CHI_SQUARED_DISTRIBUTION_H

#include <poincare/layout.h>

#include "one_parameter_distribution.h"

namespace Distributions {

class ChiSquaredDistribution : public OneParameterDistribution {
 public:
  ChiSquaredDistribution()
      : OneParameterDistribution(Poincare::Distribution::Type::Chi2) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::ChiSquareDistribution;
  }
  bool authorizedParameterAtIndex(double x, int index) const override;

 private:
  constexpr static double k_maxK = 31500.0;
  I18n::Message messageForParameterAtIndex(int index) const override {
    return I18n::Message::DegreesOfFreedomDefinition;
  }
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
