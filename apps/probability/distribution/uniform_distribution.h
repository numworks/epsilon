#ifndef PROBABILITE_UNIFORM_DISTRIBUTION_H
#define PROBABILITE_UNIFORM_DISTRIBUTION_H

#include "two_parameter_distribution.h"

namespace Probability {

class UniformDistribution final : public TwoParameterDistribution {
public:
  UniformDistribution() : TwoParameterDistribution(-1.0f, 1.0f) {}
  I18n::Message title() override { return I18n::Message::UniformDistribution; }
  Type type() const override { return Type::Uniform; }
  bool isContinuous() const override { return true; }
  float xMin() const override;
  float xMax() const override;
  float yMax() const override;
  I18n::Message parameterNameAtIndex(int index) override;
  I18n::Message parameterDefinitionAtIndex(int index) override;
  float evaluateAtAbscissa(float x) const override;
  bool authorizedValueAtIndex(float x, int index) const override;
  void setParameterAtIndex(float f, int index) override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * probability) override;
private:
  constexpr static float k_diracMaximum = 10.0f;
  constexpr static float k_diracWidth = 0.005f;
};

}

#endif
