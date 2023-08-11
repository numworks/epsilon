#ifndef INFERENCE_MODELS_STATISTIC_ONE_MEAN_Z_TEST_H
#define INFERENCE_MODELS_STATISTIC_ONE_MEAN_Z_TEST_H

#include "test.h"

namespace Inference {

class OneMeanZTest : public Test {
 public:
  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::OneMean;
  }
  DistributionType distributionType() const override {
    return DistributionType::Z;
  }
  I18n::Message title() const override { return OneMean::ZTitle(); }

  // Significance Test: OneMean
  bool initializeDistribution(DistributionType distributionType) override {
    return OneMean::TestInitializeDistribution(this, distributionType);
  }
  int numberOfAvailableDistributions() const override {
    return OneMean::NumberOfAvailableDistributions();
  }
  I18n::Message distributionTitle() const override {
    return OneMean::DistributionTitle();
  }
  const char* hypothesisSymbol() override {
    return OneMean::HypothesisSymbol();
  }
  void initParameters() override { OneMean::InitTestParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override {
    return Inference::authorizedParameterAtIndex(p, i) &&
           OneMean::ZAuthorizedParameterAtIndex(i, p);
  }
  void setParameterAtIndex(double p, int index) override {
    p = OneMean::ProcessParamaterForIndex(p, index);
    Test::setParameterAtIndex(p, index);
  }

  void compute() override { OneMean::ComputeZTest(this); }

 private:
  // Significance Test
  int numberOfStatisticParameters() const override {
    return OneMean::NumberOfParameters();
  }
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return OneMean::ZParameterRepresentationAtIndex(i);
  }
  double* parametersArray() override { return m_params; }

  double m_params[OneMean::k_numberOfParams];
};

}  // namespace Inference

#endif
