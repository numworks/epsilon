#ifndef INFERENCE_MODELS_STATISTIC_TWO_PROPORTIONS_Z_TEST_H
#define INFERENCE_MODELS_STATISTIC_TWO_PROPORTIONS_Z_TEST_H

#include "test.h"

namespace Inference {

class TwoProportionsZTest : public Test {
  friend class TwoProportions;

 public:
  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::TwoProportions;
  }
  DistributionType distributionType() const override {
    return DistributionType::Z;
  }
  I18n::Message title() const override { return TwoProportions::Title(); }
  void tidy() override { m_p1p2Layout = m_p2Layout = Poincare::Layout(); }

  // Significance Test: TwoProportions
  const char* hypothesisSymbol() const override {
    return TwoProportions::HypothesisSymbol();
  }
  void initParameters() override { TwoProportions::InitTestParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override {
    return Inference::authorizedParameterAtIndex(p, i) &&
           TwoProportions::AuthorizedParameterAtIndex(i, p);
  }
  void setParameterAtIndex(double p, int index) override {
    p = TwoProportions::ProcessParameterForIndex(p, index);
    Test::setParameterAtIndex(p, index);
  }
  bool isValidH0(double h0) override { return TwoProportions::ValidH0(h0); }

  void compute() override { TwoProportions::ComputeTest(this); }

  // Estimates
  int numberOfEstimates() const override { return 3; }
  double estimateValue(int index) override;
  Poincare::Layout estimateLayout(int index) const override;
  I18n::Message estimateDescription(int index) override;

 private:
  // Significance Test: TwoProportions
  bool validateInputs() override {
    return TwoProportions::ValidateInputs(m_params);
  }
  int numberOfStatisticParameters() const override {
    return TwoProportions::NumberOfParameters();
  }
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return TwoProportions::ParameterRepresentationAtIndex(i);
  }
  double* parametersArray() override { return m_params; }

  double m_params[TwoProportions::k_numberOfParams];
  enum class EstimatesOrder { P1, P2, Pooled };
  mutable Poincare::Layout m_p1p2Layout;
  mutable Poincare::Layout m_p2Layout;
};

}  // namespace Inference

#endif
