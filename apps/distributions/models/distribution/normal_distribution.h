#ifndef PROBABILITE_NORMAL_DISTRIBUTION_H
#define PROBABILITE_NORMAL_DISTRIBUTION_H

#include "poincare/statistics/distribution.h"
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
  I18n::Message messageForParameterAtIndex(int index) const override {
    switch (index) {
      case Poincare::Distribution::Params::Normal::Mu:
        return I18n::Message::MeanDefinition;
      case Poincare::Distribution::Params::Normal::Sigma:
        return I18n::Message::StandardDeviationDefinition;
      default:
        OMG::unreachable();
    }
  }
  float privateXExtremum(bool min) const;
  float privateComputeXMin() const override { return privateXExtremum(true); }
  float privateComputeXMax() const override { return privateXExtremum(false); }
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
