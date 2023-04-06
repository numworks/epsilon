#ifndef PROBABILITE_FISHER_DISTRIBUTION_H
#define PROBABILITE_FISHER_DISTRIBUTION_H

#include "two_parameters_distribution.h"

namespace Distributions {

class FisherDistribution final : public TwoParametersDistribution {
 public:
  FisherDistribution()
      : TwoParametersDistribution(Poincare::Distribution::Type::Fisher,
                                  k_defaultD1, k_defaultD2) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::FisherDistribution;
  }
  const char* parameterNameAtIndex(int index) const override {
    return index == 0 ? "d1" : "d2";
  }
  bool authorizedParameterAtIndex(double x, int index) const override;
  double defaultParameterAtIndex(int index) const override {
    return index == 0 ? k_defaultD1 : k_defaultD2;
  }

 private:
  constexpr static double k_defaultD1 = 1.0;
  constexpr static double k_defaultD2 = 1.0;
  // The display works badly for d1 = d2 > 144.
  constexpr static double k_maxParameter = 144.0;
  constexpr static float k_defaultMax = 3.0f;
  enum ParamsOrder { D1, D2 };
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override;
  float mode() const;
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
