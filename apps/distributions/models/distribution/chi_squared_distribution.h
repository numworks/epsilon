#ifndef DISTRIBUTION_CHI_SQUARED_DISTRIBUTION_H
#define DISTRIBUTION_CHI_SQUARED_DISTRIBUTION_H

#include <poincare/layout.h>

#include "one_parameter_distribution.h"

namespace Distributions {

class ChiSquaredDistribution : public OneParameterDistribution {
 public:
  ChiSquaredDistribution()
      : OneParameterDistribution(Poincare::Distribution::Type::Chi2,
                                 k_defaultK) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::ChiSquareDistribution;
  }
  const char* parameterNameAtIndex(int index) const override { return "k"; }
  double defaultParameterAtIndex(int index) const override {
    return k_defaultK;
  }

 private:
  constexpr static double k_maxK = 31500.0;
  constexpr static double k_defaultK = 1.0;
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return Shared::ParameterRepresentation{
        Poincare::Layout::String(parameterNameAtIndex(0)),
        I18n::Message::DegreesOfFreedomDefinition};
  }
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
