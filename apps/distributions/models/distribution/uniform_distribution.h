#ifndef PROBABILITE_UNIFORM_DISTRIBUTION_H
#define PROBABILITE_UNIFORM_DISTRIBUTION_H

#include "poincare/statistics/distribution.h"
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

  float evaluateAtAbscissa(float x) const override;
  void setParameterAtIndex(double f, int index) override;

 private:
  constexpr static float k_diracMaximum = 10.0f;
  constexpr static float k_diracWidth = 0.005f;

  I18n::Message messageForParameterAtIndex(int index) const override {
    switch (index) {
      case Poincare::Distribution::Params::Uniform::A:
        return I18n::Message::IntervalADescr;
      case Poincare::Distribution::Params::Uniform::B:
        return I18n::Message::IntervalBDescr;
      default:
        OMG::unreachable();
    }
  }

  float privateComputeXMin() const override;
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
