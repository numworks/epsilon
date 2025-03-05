#ifndef PROBABILITE_UNIFORM_DISTRIBUTION_H
#define PROBABILITE_UNIFORM_DISTRIBUTION_H

#include "two_parameters_distribution.h"

namespace Distributions {

class UniformDistribution final : public TwoParametersDistribution {
 public:
  UniformDistribution()
      : TwoParametersDistribution(Poincare::Distribution::Type::Uniform) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::UniformDistribution;
  }

  void setParameterAtIndex(double f, int index) override;

 private:
  enum ParamsOrder { A, B };
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override;
  float privateComputeXMin() const override;
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
