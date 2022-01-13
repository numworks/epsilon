#ifndef PROBABILITE_GEOMETRIC_DISTRIBUTION_H
#define PROBABILITE_GEOMETRIC_DISTRIBUTION_H

#include "one_parameter_distribution.h"

namespace Probability {

/* We chose the definition:
 * 0 < p <= 1 for probability of success
 * k number of trials needed to get one success, where k ∈ {1, 2, 3, ...}. */

class GeometricDistribution final : public OneParameterDistribution {
public:
  GeometricDistribution() : OneParameterDistribution(0.5) {}
  I18n::Message title() override { return I18n::Message::GeometricDistribution; }
  Type type() const override { return Type::Geometric; }
  bool isContinuous() const override { return false; }
  I18n::Message parameterNameAtIndex(int index) override {
    assert(index == 0);
    return I18n::Message::P;
  }
  I18n::Message parameterDefinitionAtIndex(int index) override  {
    assert(index == 0);
    return I18n::Message::SuccessProbability;
  }
  float evaluateAtAbscissa(float x) const override {
    return templatedApproximateAtAbscissa<float>(x);
  }
  bool authorizedValueAtIndex(double x, int index) const override;
  double defaultComputedValue() const override { return 1.0; }
private:
  double evaluateAtDiscreteAbscissa(int k) const override {
    return templatedApproximateAtAbscissa<double>(static_cast<double>(k));
  }
  template<typename T> T templatedApproximateAtAbscissa(T x) const;
  float computeXMin() const override;
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
