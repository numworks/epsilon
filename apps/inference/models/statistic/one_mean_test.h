#ifndef INFERENCE_MODELS_STATISTIC_ONE_MEAN_TEST_H
#define INFERENCE_MODELS_STATISTIC_ONE_MEAN_TEST_H

#include "one_mean_statistic.h"
#include "test.h"

namespace Inference {

class OneMeanTest : public Test, public OneMeanStatistic {
 public:
  using OneMeanStatistic::OneMeanStatistic;

  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::OneMean;
  }
  I18n::Message title() const override { return OneMean::Title(oneMeanType()); }

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
  bool authorizedParameterAtPosition(double p, int row,
                                     int column) const override {
    return authorizedParameterAtIndex(p, index2DToIndex(row, column));
  }
  bool authorizedParameterAtIndex(double p, int i) const override {
    return Inference::authorizedParameterAtIndex(p, i) &&
           OneMean::AuthorizedParameterAtIndex(oneMeanType(), i, p);
  }
  void setParameterAtIndex(double p, int index) override {
    p = OneMean::ProcessParamaterForIndex(p, index);
    Test::setParameterAtIndex(p, index);
  }

  void compute() override { OneMean::ComputeTest(oneMeanType(), this); }

 private:
  OneMean::Type oneMeanType() const {
    return OneMeanStatistic::OneMeanType(this);
  }

  // Significance Test
  int numberOfStatisticParameters() const override {
    return OneMean::NumberOfParameters();
  }
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return OneMean::ParameterRepresentationAtIndex(oneMeanType(), i);
  }
  double* parametersArray() override { return m_params; }
};

class OneMeanTTest : public OneMeanTest {
 public:
  using OneMeanTest::OneMeanTest;

  DistributionType distributionType() const override {
    return DistributionType::T;
  }
};

class OneMeanZTest : public OneMeanTest {
 public:
  using OneMeanTest::OneMeanTest;

  DistributionType distributionType() const override {
    return DistributionType::Z;
  }
};

}  // namespace Inference

#endif
