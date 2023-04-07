#ifndef PROBABILITE_NORMAL_DISTRIBUTION_H
#define PROBABILITE_NORMAL_DISTRIBUTION_H

#include <poincare/normal_distribution.h>

#include "two_parameters_distribution.h"

namespace Distributions {

class NormalDistribution final : public TwoParametersDistribution {
 public:
  NormalDistribution()
      : TwoParametersDistribution(Poincare::Distribution::Type::Normal,
                                  k_defaultMu, k_defaultSigma) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::NormalDistribution;
  }
  const char* parameterNameAtIndex(int index) const override {
    return index == 0 ? "μ" : "σ";
  }
  double meanAbscissa() override { return m_parameters[0]; }
  bool authorizedParameterAtIndex(double x, int index) const override;
  double defaultParameterAtIndex(int index) const override {
    return index == 0 ? k_defaultMu : k_defaultSigma;
  }
  void setParameterAtIndex(double f, int index) override;
  bool canHaveUninitializedParameter() const override { return true; }

 private:
  constexpr static double k_defaultMu =
      Poincare::NormalDistribution::k_standardMu;
  constexpr static double k_defaultSigma =
      Poincare::NormalDistribution::k_standardSigma;
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
