#ifndef PROBABILITE_UNIFORM_DISTRIBUTION_H
#define PROBABILITE_UNIFORM_DISTRIBUTION_H

#include "two_parameter_distribution.h"

namespace Probability {

class UniformDistribution final : public TwoParameterDistribution {
public:
  UniformDistribution() : TwoParameterDistribution(-1.0, 1.0) {}
  I18n::Message title() override { return I18n::Message::UniformDistribution; }
  Type type() const override { return Type::Uniform; }
  bool isContinuous() const override { return true; }
  I18n::Message parameterNameAtIndex(int index) override;
  I18n::Message parameterDefinitionAtIndex(int index) override;
  float evaluateAtAbscissa(float x) const override;
  bool authorizedValueAtIndex(double x, int index) const override;
  void setParameterAtIndex(double f, int index) override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * probability) override;
private:
  constexpr static float k_diracMaximum = 10.0f;
  constexpr static float k_diracWidth = 0.005f;
  float computeXMin() const override;
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
