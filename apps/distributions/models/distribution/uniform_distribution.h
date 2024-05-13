#ifndef PROBABILITE_UNIFORM_DISTRIBUTION_H
#define PROBABILITE_UNIFORM_DISTRIBUTION_H

#include "two_parameters_distribution.h"

namespace Distributions {

class UniformDistribution final : public TwoParametersDistribution {
 public:
  UniformDistribution()
      : TwoParametersDistribution(Poincare::Distribution::Type::Uniform,
                                  k_defaultA, k_defaultB) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::UniformDistribution;
  }
  const char* parameterNameAtIndex(int index) const override {
    return index == 0 ? "a" : "b";
  }
  double meanAbscissa() override {
    return (m_parameters[0] + m_parameters[1]) / 2.0;
  }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedParameterAtIndex(double x, int index) const override;
  double defaultParameterAtIndex(int index) const override {
    return index == 0 ? k_defaultA : k_defaultB;
  }
  void setParameterAtIndex(double f, int index) override;

 private:
  constexpr static double k_defaultA = -1.0;
  constexpr static double k_defaultB = 1.0;
  constexpr static float k_diracMaximum = 10.0f;
  constexpr static float k_diracWidth = 0.005f;
  enum ParamsOrder { A, B };
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override;
  float privateComputeXMin() const override;
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
