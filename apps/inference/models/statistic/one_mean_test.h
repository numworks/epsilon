#ifndef INFERENCE_MODELS_STATISTIC_ONE_MEAN_TEST_H
#define INFERENCE_MODELS_STATISTIC_ONE_MEAN_TEST_H

#include "one_mean_statistic.h"
#include "test.h"

namespace Inference {

class OneMeanTest : public Test, public OneMeanStatistic {
 public:
  using OneMeanStatistic::OneMeanStatistic;

  void init() override { initDatasetsIfSeries(); }
  void tidy() override { tidyDatasets(); }

  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::OneMean;
  }
  I18n::Message title() const override {
    return OneMean::Title(oneMeanType(this));
  }

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
  const char* hypothesisSymbol() const override {
    return OneMean::HypothesisSymbol();
  }
  void initParameters() override { OneMean::InitTestParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override {
    return Inference::authorizedParameterAtIndex(p, i) &&
           OneMean::AuthorizedParameterAtIndex(oneMeanType(this), i, p);
  }
  void setParameterAtIndex(double p, int index) override {
    p = OneMean::ProcessParameterForIndex(p, index);
    Test::setParameterAtIndex(p, index);
  }
  bool validateInputs() override { return parametersAreValid(this); }
  int numberOfResults() const override {
    return numberOfResultsAndComputedParameters(this, Test::numberOfResults());
  }
  int secondResultSectionStart() const override {
    return numberOfStatisticParameters();
  }
  void resultAtIndex(int index, double* value, Poincare::Layout* message,
                     I18n::Message* subMessage, int* precision) override {
    if (!computedParameterAtIndex(&index, this, value, message, subMessage,
                                  precision)) {
      Test::resultAtIndex(index, value, message, subMessage, precision);
    }
  }

  void compute() override {
    syncParametersWithStore(this);
    OneMean::ComputeTest(oneMeanType(this), this);
  }

 private:
  // Significance Test
  int numberOfStatisticParameters() const override {
    return OneMean::NumberOfParameters();
  }
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return OneMean::ParameterRepresentationAtIndex(oneMeanType(this), i);
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
