#ifndef PROBABILITE_POISSON_DISTRIBUTION_H
#define PROBABILITE_POISSON_DISTRIBUTION_H

#include "one_parameter_distribution.h"

namespace Probability {

class PoissonDistribution final : public OneParameterDistribution {
public:
  PoissonDistribution() : OneParameterDistribution(4.0f) {}
  I18n::Message title() override { return I18n::Message::PoissonDistribution; }
  Type type() const override { return Type::Poisson; }
  bool isContinuous() const override { return false; }
  float xMin() const override;
  float xMax() const override;
  float yMax() const override;
  I18n::Message parameterNameAtIndex(int index) override {
    assert(index == 0);
    return I18n::Message::Lambda;
  }
  I18n::Message parameterDefinitionAtIndex(int index) override  {
    assert(index == 0);
    return I18n::Message::LambdaPoissonDefinition;
  }
  float evaluateAtAbscissa(float x) const override {
    return templatedApproximateAtAbscissa<float>(x);
  }
  bool authorizedValueAtIndex(float x, int index) const override;
private:
  double evaluateAtDiscreteAbscissa(int k) const override {
    return templatedApproximateAtAbscissa<double>(static_cast<double>(k));
  }
  template<typename T> T templatedApproximateAtAbscissa(T x) const;
};

}

#endif
