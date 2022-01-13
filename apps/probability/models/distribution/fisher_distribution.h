#ifndef PROBABILITE_FISHER_DISTRIBUTION_H
#define PROBABILITE_FISHER_DISTRIBUTION_H

#include "two_parameter_distribution.h"

namespace Probability {

class FisherDistribution final : public TwoParameterDistribution {
public:
  FisherDistribution() : TwoParameterDistribution(1.0, 1.0) {}
  I18n::Message title() override { return I18n::Message::FisherDistribution; }
  Type type() const override { return Type::Fisher; }
  bool isContinuous() const override { return true; }
  I18n::Message parameterNameAtIndex(int index) override;
  I18n::Message parameterDefinitionAtIndex(int index) override;
  float evaluateAtAbscissa(float x) const override;
  bool authorizedValueAtIndex(double x, int index) const override;
  void setParameterAtIndex(double f, int index) override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * probability) override;
private:
  constexpr static double k_maxParameter = 144.0; // The display works badly for d1 = d2 > 144.
  constexpr static float k_defaultMax = 3.0f;
  float mode() const;
  float computeXMin() const override;
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
