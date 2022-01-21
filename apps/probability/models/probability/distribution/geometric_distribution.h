#ifndef PROBABILITE_GEOMETRIC_DISTRIBUTION_H
#define PROBABILITE_GEOMETRIC_DISTRIBUTION_H

#include "one_parameter_distribution.h"
#include <poincare/code_point_layout.h>

namespace Probability {

/* We chose the definition:
 * 0 < p <= 1 for distribution of success
 * k number of trials needed to get one success, where k ∈ {1, 2, 3, ...}. */

class GeometricDistribution final : public OneParameterDistribution {
public:
  GeometricDistribution() : OneParameterDistribution(0.5) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::GeometricDistribution; }
  Type type() const override { return Type::Geometric; }
  bool isContinuous() const override { return false; }
  bool isSymmetrical() const override { return false; }
  float evaluateAtAbscissa(float x) const override {
    return templatedApproximateAtAbscissa<float>(x);
  }
  bool authorizedParameterAtIndex(double x, int index) const override;
  double defaultComputedValue() const override { return 1.0; }
private:
  ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return ParameterRepresentation{Poincare::CodePointLayout::Builder('p'), I18n::Message::SuccessProbability};
  }
  double evaluateAtDiscreteAbscissa(int k) const override {
    return templatedApproximateAtAbscissa<double>(static_cast<double>(k));
  }
  template<typename T> T templatedApproximateAtAbscissa(T x) const;
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
