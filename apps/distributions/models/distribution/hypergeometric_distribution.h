#ifndef PROBABILITE_HYPERGEOMETRIC_DISTRIBUTION_H
#define PROBABILITE_HYPERGEOMETRIC_DISTRIBUTION_H

#include <poincare/layout_helper.h>

#include "three_parameters_distribution.h"

namespace Distributions {

class HypergeometricDistribution final : public ThreeParametersDistribution {
 public:
  HypergeometricDistribution()
      : ThreeParametersDistribution(
            Poincare::Distribution::Type::Hypergeometric, k_defaultN,
            k_defaultK, k_defaultn) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::HypergeometricDistribution;
  }
  const char* parameterNameAtIndex(int index) const override {
    const char* parametersNames[] = {"N", "K", "n"};
    return parametersNames[index];
  }
  bool authorizedParameterAtIndex(double x, int index) const override;
  void setParameterAtIndex(double f, int index) override;
  double defaultParameterAtIndex(int index) const override {
    return index == 0 ? k_defaultN : (index == 1 ? k_defaultK : k_defaultn);
  }
  double defaultComputedValue() const override { return 1.0; }

 private:
  constexpr static double k_defaultN = 100.;
  constexpr static double k_defaultK = 60.;
  constexpr static double k_defaultn = 50.;
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    I18n::Message parameterTexts[] = {I18n::Message::PopulationSize,
                                      I18n::Message::TotalItemsWithFeature,
                                      I18n::Message::SampleSize};
    return Shared::ParameterRepresentation{
        Poincare::LayoutHelper::String(parameterNameAtIndex(i)),
        parameterTexts[i]};
  }
  float computeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
