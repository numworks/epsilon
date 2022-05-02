#ifndef PROBABILITE_BINOMIAL_DISTRIBUTION_H
#define PROBABILITE_BINOMIAL_DISTRIBUTION_H

#include "two_parameter_distribution.h"

namespace Inference {

class BinomialDistribution final : public TwoParameterDistribution {
public:
  BinomialDistribution() : TwoParameterDistribution(20.0, 0.5) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::BinomialDistribution; }
  Type type() const override { return Type::Binomial; }
  const char * parameterNameAtIndex(int index) const override { return index == 0 ? "n" : "p"; }
  bool isContinuous() const override { return false; }
  bool isSymmetrical() const override { return false; }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedParameterAtIndex(double x, int index) const override;
  double cumulativeDistributiveInverseForProbability(double * p) override;
  double rightIntegralInverseForProbability(double * p) override;
protected:
  enum ParamsOrder { N, P };
  ParameterRepresentation paramRepresentationAtIndex(int i) const override;
  float computeXMin() const override;
  float computeXMax() const override;
  float computeYMax() const override;
  double evaluateAtDiscreteAbscissa(int k) const override;
};

}

#endif
