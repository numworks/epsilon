#ifndef PROBABILITE_BINOMIAL_DISTRIBUTION_H
#define PROBABILITE_BINOMIAL_DISTRIBUTION_H

#include "two_parameter_distribution.h"

namespace Probability {

class BinomialDistribution final : public TwoParameterDistribution {
public:
  BinomialDistribution() : TwoParameterDistribution(20.0, 0.5) { computeCurveViewRange(); }
  I18n::Message title() override { return I18n::Message::BinomialDistribution; }
  Type type() const override { return Type::Binomial; }
  bool isContinuous() const override { return false; }
  bool isSymetrical() const override { return true; }
  I18n::Message parameterNameAtIndex(int index) override;
  I18n::Message parameterDefinitionAtIndex(int index) override;
  float evaluateAtAbscissa(float x) const override;
  bool authorizedValueAtIndex(double x, int index) const override;
  double cumulativeDistributiveInverseForProbability(double * probability) override;
  double rightIntegralInverseForProbability(double * probability) override;
protected:
  double evaluateAtDiscreteAbscissa(int k) const override;
  float computeXMin() const override;
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
