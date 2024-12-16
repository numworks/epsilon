#ifndef INFERENCE_MODELS_STATISTIC_TWO_MEANS_TEST_H
#define INFERENCE_MODELS_STATISTIC_TWO_MEANS_TEST_H

#include "test.h"
#include "two_means_statistic.h"

namespace Inference {

class TwoMeansTest : public Test, public TwoMeansStatistic {
 public:
  using TwoMeansStatistic::TwoMeansStatistic;

  void init() override { initDatasetsIfSeries(); }
  void tidy() override { tidyDatasets(); }

  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::TwoMeans;
  }
  I18n::Message title() const override {
    return TwoMeans::Title(twoMeansType(this));
  }

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
  const char* hypothesisSymbol() const override {
    return TwoMeans::HypothesisSymbol();
  }
  void initParameters() override { TwoMeans::InitTestParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override {
    return Inference::authorizedParameterAtIndex(p, i) &&
           TwoMeans::AuthorizedParameterAtIndex(twoMeansType(this), i, p);
  }
  void setParameterAtIndex(double p, int index) override {
    p = TwoMeans::ProcessParameterForIndex(p, index);
    Test::setParameterAtIndex(p, index);
  }
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
    TwoMeans::ComputeTest(twoMeansType(this), this);
  }

 private:
  // Significance Test: TwoMeans
  bool validateInputs() override {
    return parametersAreValid(this) &&
           TwoMeans::ValidateInputs(twoMeansType(this), m_params);
  }
  int numberOfStatisticParameters() const override {
    return TwoMeans::NumberOfParameters();
  }
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return TwoMeans::ParameterRepresentationAtIndex(twoMeansType(this), i);
  }
  double* parametersArray() override { return m_params; }
};

class TwoMeansTTest : public TwoMeansTest {
 public:
  using TwoMeansTest::TwoMeansTest;

  DistributionType distributionType() const override {
    return DistributionType::T;
  }
};

class PooledTwoMeansTTest : public TwoMeansTest {
 public:
  using TwoMeansTest::TwoMeansTest;

  DistributionType distributionType() const override {
    return DistributionType::TPooled;
  }
};

class TwoMeansZTest : public TwoMeansTest {
 public:
  using TwoMeansTest::TwoMeansTest;

  DistributionType distributionType() const override {
    return DistributionType::Z;
  }
};

};  // namespace Inference

#endif
