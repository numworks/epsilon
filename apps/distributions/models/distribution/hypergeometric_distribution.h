#ifndef PROBABILITE_HYPERGEOMETRIC_DISTRIBUTION_H
#define PROBABILITE_HYPERGEOMETRIC_DISTRIBUTION_H

#include <poincare/layout.h>

#include "apps/i18n.h"
#include "three_parameters_distribution.h"

namespace Distributions {

class HypergeometricDistribution final : public ThreeParametersDistribution {
 public:
  HypergeometricDistribution()
      : ThreeParametersDistribution(
            Poincare::Distribution::Type::Hypergeometric) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::HypergeometricDistribution;
  }

  void setParameterAtIndex(double f, int index) override;
  double defaultComputedValue() const override { return 1.0; }

 private:
  I18n::Message messageForParameterAtIndex(int i) const override {
    I18n::Message parameterTexts[] = {I18n::Message::PopulationSize,
                                      I18n::Message::TotalItemsWithFeature,
                                      I18n::Message::SampleSize};
    return parameterTexts[i];
  }
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
