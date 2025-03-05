#ifndef PROBABILITE_FISHER_DISTRIBUTION_H
#define PROBABILITE_FISHER_DISTRIBUTION_H

#include "two_parameters_distribution.h"

namespace Distributions {

class FisherDistribution final : public TwoParametersDistribution {
 public:
  FisherDistribution()
      : TwoParametersDistribution(Poincare::Distribution::Type::Fisher) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::FisherDistribution;
  }
  bool authorizedParameterAtIndex(double x, int index) const override;

 private:
  // The display works badly for d1 = d2 > 144.
  constexpr static double k_maxParameter = 144.0;
  constexpr static float k_defaultMax = 3.0f;
  enum ParamsOrder { D1, D2 };
  I18n::Message messageForParameterAtIndex(int index) const override {
    switch (index) {
      case ParamsOrder::D1:
        return I18n::Message::D1FisherDefinition;
      case ParamsOrder::D2:
        return I18n::Message::D2FisherDefinition;
      default:
        OMG::unreachable();
    }
  }
  float mode() const;
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
