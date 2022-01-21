#ifndef PROBABILITE_UNIFORM_DISTRIBUTION_H
#define PROBABILITE_UNIFORM_DISTRIBUTION_H

#include "two_parameter_distribution.h"

namespace Probability {

class UniformDistribution final : public TwoParameterDistribution {
public:
  UniformDistribution() : TwoParameterDistribution(-1.0, 1.0) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::UniformDistribution; }
  Type type() const override { return Type::Uniform; }
  bool isContinuous() const override { return true; }
  bool isSymmetrical() const override { return true; }
  double meanAbscissa() override { return (m_parameters[0] + m_parameters[1]) / 2.0; }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedParameterAtIndex(double x, int index) const override;
  void setParameterAtIndex(double f, int index) override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * distribution) override;
private:
  constexpr static float k_diracMaximum = 10.0f;
  constexpr static float k_diracWidth = 0.005f;
  enum ParamsOrder { A, B };
  ParameterRepresentation paramRepresentationAtIndex(int i) const override;
  float computeXMin() const override;
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
