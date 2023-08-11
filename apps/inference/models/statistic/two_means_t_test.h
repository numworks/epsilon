#ifndef INFERENCE_MODELS_STATISTIC_TWO_MEANS_T_TEST_H
#define INFERENCE_MODELS_STATISTIC_TWO_MEANS_T_TEST_H

#include "test.h"

namespace Inference {

class TwoMeansTTest : public Test {
  friend class TwoMeans;

 public:
  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::TwoMeans;
  }
  DistributionType distributionType() const override {
    return DistributionType::T;
  }
  I18n::Message title() const override { return TwoMeans::TTitle(); }

  // Significance Test: TwoMeans
  bool initializeDistribution(DistributionType distributionType) override {
    return TwoMeans::TestInitializeDistribution(this, distributionType);
  }
  int numberOfAvailableDistributions() const override {
    return TwoMeans::NumberOfAvailableDistributions();
  }
  I18n::Message distributionTitle() const override {
    return TwoMeans::DistributionTitle();
  }
  const char* hypothesisSymbol() override {
    return TwoMeans::HypothesisSymbol();
  }
  void initParameters() override { TwoMeans::InitTestParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override {
    return Inference::authorizedParameterAtIndex(p, i) &&
           TwoMeans::TAuthorizedParameterAtIndex(i, p);
  }
  void setParameterAtIndex(double p, int index) override {
    p = TwoMeans::ProcessParamaterForIndex(p, index);
    Test::setParameterAtIndex(p, index);
  }

  void compute() override { TwoMeans::ComputeTTest(this); }

 private:
  // Significance Test: TwoMeans
  bool validateInputs() override { return TwoMeans::TValidateInputs(m_params); }
  int numberOfStatisticParameters() const override {
    return TwoMeans::NumberOfParameters();
  }
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return TwoMeans::TParameterRepresentationAtIndex(i);
  }
  double* parametersArray() override { return m_params; }

  double m_params[TwoMeans::k_numberOfParams];
};

}  // namespace Inference

#endif
