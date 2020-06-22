#ifndef PROBABILITE_GEOMETRIC_DISTRIBUTION_H
#define PROBABILITE_GEOMETRIC_DISTRIBUTION_H

#include "one_parameter_distribution.h"

namespace Probability {

/* We chose the definition:
 * 0 < p <= 1 for success probability
 * k number of trials needed to get one success, where k ∈ {1, 2, 3, ...}. */

class GeometricDistribution final : public OneParameterDistribution {
public:
  GeometricDistribution() : OneParameterDistribution(0.5f) {}
  I18n::Message title() override { return I18n::Message::GeometricDistribution; }
  Type type() const override { return Type::Geometric; }
  bool isContinuous() const override { return false; }
  float xMin() const override;
  float xMax() const override;
  float yMax() const override;
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
  bool authorizedValueAtIndex(float x, int index) const override;
  double defaultComputedValue() const override { return 1.0; }
private:
  double evaluateAtDiscreteAbscissa(int k) const override {
    return templatedApproximateAtAbscissa<double>(static_cast<double>(k));
  }
  template<typename T> T templatedApproximateAtAbscissa(T x) const;
};

}

#endif
