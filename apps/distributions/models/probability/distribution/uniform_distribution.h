#ifndef PROBABILITE_UNIFORM_DISTRIBUTION_H
#define PROBABILITE_UNIFORM_DISTRIBUTION_H

#include "two_parameters_distribution.h"

namespace Inference {

class UniformDistribution final : public TwoParametersDistribution {
public:
  UniformDistribution() : TwoParametersDistribution(Poincare::Distribution::Type::Uniform, -1.0, 1.0) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::UniformDistribution; }
  const char * parameterNameAtIndex(int index) const override { return index == 0 ? "a" : "b"; }
  double meanAbscissa() override { return (m_parameters[0] + m_parameters[1]) / 2.0; }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedParameterAtIndex(double x, int index) const override;
  void setParameterAtIndex(double f, int index) override;
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
