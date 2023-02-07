#ifndef PROBABILITE_BINOMIAL_DISTRIBUTION_H
#define PROBABILITE_BINOMIAL_DISTRIBUTION_H

#include "two_parameters_distribution.h"

namespace Distributions {

class BinomialDistribution final : public TwoParametersDistribution {
public:
  BinomialDistribution() : TwoParametersDistribution(Poincare::Distribution::Type::Binomial, k_defaultN, k_defaultP) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::BinomialDistribution; }
  const char * parameterNameAtIndex(int index) const override { return index == 0 ? "n" : "p"; }
  bool authorizedParameterAtIndex(double x, int index) const override;
  double defaultParameterAtIndex(int index) const override { return index == 0 ? k_defaultN : k_defaultP; }
  double cumulativeDistributiveInverseForProbability(double p) const override;
  double rightIntegralInverseForProbability(double p) const override;
protected:
  enum ParamsOrder { N, P };
  constexpr static double k_defaultN = 20.0;
  constexpr static double k_defaultP = 0.5;
  Shared::ParameterRepresentation paramRepresentationAtIndex(int i) const override;
  float computeXMin() const override;
  float computeXMax() const override;
  float computeYMax() const override;
  double evaluateAtDiscreteAbscissa(int k) const override;
};

}

#endif
