#ifndef PROBABILITE_NORMAL_DISTRIBUTION_H
#define PROBABILITE_NORMAL_DISTRIBUTION_H

#include "two_parameters_distribution.h"

namespace Distributions {

class NormalDistribution final : public TwoParametersDistribution {
 public:
  NormalDistribution()
      : TwoParametersDistribution(Poincare::Distribution::Type::Normal) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::NormalDistribution;
  }
  bool authorizedParameterAtIndex(double x, int index) const override;
  void setParameterAtIndex(double f, int index) override;
  bool canHaveUninitializedParameter() const override { return true; }

 private:
  constexpr static double k_maxRatioMuSigma = 1000000.0f;
  enum ParamsOrder { Mu, Sigma };
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override;
  float privateXExtremum(bool min) const;
  float privateComputeXMin() const override { return privateXExtremum(true); }
  float privateComputeXMax() const override { return privateXExtremum(false); }
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
