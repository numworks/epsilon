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

  double defaultParameterAtIndex(int index) const override {
    return index == 0 ? k_defaultA : k_defaultB;
  }
  void setParameterAtIndex(double f, int index) override;

 private:
  constexpr static double k_defaultA = -1.0;
  constexpr static double k_defaultB = 1.0;

  enum ParamsOrder { A, B };
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override;
  float privateComputeXMin() const override;
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
