#ifndef PROBABILITE_NORMAL_DISTRIBUTION_H
#define PROBABILITE_NORMAL_DISTRIBUTION_H

#include "two_parameter_distribution.h"

namespace Probability {

class NormalDistribution final : public TwoParameterDistribution {
public:
  NormalDistribution() : TwoParameterDistribution(0.0, 1.0) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::NormalDistribution; }
  Type type() const override { return Type::Normal; }
  const char * parameterNameAtIndex(int index) const override { return index == 0 ? "μ" : "σ"; }
  bool isContinuous() const override { return true; }
  bool isSymmetrical() const override { return true; }
  double meanAbscissa() override { return m_parameters[0]; }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedParameterAtIndex(double x, int index) const override;
  void setParameterAtIndex(double f, int index) override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * p) override;
private:
  constexpr static double k_maxRatioMuSigma = 1000000.0f;
  enum ParamsOrder { Mu, Sigma };
  ParameterRepresentation paramRepresentationAtIndex(int i) const override;
  float xExtremum(bool min) const;
  float computeXMin() const override { return xExtremum(true); }
  float computeXMax() const override { return xExtremum(false); }
  float computeYMax() const override;
};

}

#endif
