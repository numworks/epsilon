#ifndef PROBABILITE_GEOMETRIC_DISTRIBUTION_H
#define PROBABILITE_GEOMETRIC_DISTRIBUTION_H

#include "one_parameter_distribution.h"
#include <poincare/code_point_layout.h>
#include <poincare/layout_helper.h>

namespace Distributions {

/* We chose the definition:
 * 0 < p <= 1 for distribution of success
 * k number of trials needed to get one success, where k ∈ {1, 2, 3, ...}. */

class GeometricDistribution final : public OneParameterDistribution {
public:
  GeometricDistribution() : OneParameterDistribution(Poincare::Distribution::Type::Geometric, k_defaultP) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::GeometricDistribution; }
  const char * parameterNameAtIndex(int index) const override { return "p"; }
  bool authorizedParameterAtIndex(double x, int index) const override;
  double defaultParameterAtIndex(int index) const override { return k_defaultP; }
  double defaultComputedValue() const override { return 1.0; }
private:
  constexpr static double k_defaultP = 0.5;
  Shared::ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return Shared::ParameterRepresentation{Poincare::LayoutHelper::String(parameterNameAtIndex(0)), I18n::Message::SuccessProbability};
  }
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
