#ifndef PROBABILITE_GEOMETRIC_DISTRIBUTION_H
#define PROBABILITE_GEOMETRIC_DISTRIBUTION_H

#include <poincare/layout.h>

#include "one_parameter_distribution.h"

namespace Distributions {

/* We chose the definition:
 * 0 < p <= 1 for distribution of success
 * k number of trials needed to get one success, where k ∈ {1, 2, 3, ...}. */

class GeometricDistribution final : public OneParameterDistribution {
 public:
  GeometricDistribution()
      : OneParameterDistribution(Poincare::Distribution::Type::Geometric) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::GeometricDistribution;
  }

  double defaultComputedValue() const override { return 1.0; }

 private:
  I18n::Message messageForParameterAtIndex(int index) const override {
    return I18n::Message::SuccessProbability;
  }
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
