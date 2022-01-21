#ifndef PROBABILITE_FISHER_DISTRIBUTION_H
#define PROBABILITE_FISHER_DISTRIBUTION_H

#include "two_parameter_distribution.h"

namespace Probability {

class FisherDistribution final : public TwoParameterDistribution {
public:
  FisherDistribution() : TwoParameterDistribution(1.0, 1.0) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::FisherDistribution; }
  Type type() const override { return Type::Fisher; }
  bool isContinuous() const override { return true; }
  bool isSymmetrical() const override { return false; }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedParameterAtIndex(double x, int index) const override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * p) override;
private:
  constexpr static double k_maxParameter = 144.0; // The display works badly for d1 = d2 > 144.
  constexpr static float k_defaultMax = 3.0f;
  enum ParamsOrder { D1, D2 };
  ParameterRepresentation paramRepresentationAtIndex(int i) const override;
  float mode() const;
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
