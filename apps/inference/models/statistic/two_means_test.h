#ifndef INFERENCE_MODELS_STATISTIC_TWO_MEANS_TEST_H
#define INFERENCE_MODELS_STATISTIC_TWO_MEANS_TEST_H

#include "inference/models/statistic/table_from_store.h"
#include "test.h"
#include "two_means_statistic.h"

namespace Inference {

class TwoMeansTest : public Test, public TwoMeansStatistic {
 public:
  using TwoMeansStatistic::TwoMeansStatistic;
  Table* table() override { return this; }
  void init() override { initDatasetsIfSeries(); }
  void tidy() override { tidyDatasets(); }

  constexpr PcrInference::TestType testType() const override {
    return PcrInference::TestType::TwoMeans;
  }

  double preProcessParameter(double p, int index) const override {
    return preProcessTwoMeansParameter(p, index);
  }

  bool validateInputs(int pageIndex) override {
    return TableFromStatisticStore::validateInputs(this, pageIndex);
  }

 private:
  int numberOfExtraResults() const override {
    return numberOfComputedParameters(this);
  }
  void extraResultAtIndex(int index, double* value, Poincare::Layout* message,
                          I18n::Message* subMessage, int* precision) override {
    computedParameterAtIndex(index, this, value, message, subMessage,
                             precision);
  }
  double* parametersArray() override { return m_params; }
};

class TwoMeansTTest : public TwoMeansTest {
 public:
  using TwoMeansTest::TwoMeansTest;

  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::T;
  }
};

class PooledTwoMeansTTest : public TwoMeansTest {
 public:
  using TwoMeansTest::TwoMeansTest;

  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::TPooled;
  }
};

class TwoMeansZTest : public TwoMeansTest {
 public:
  using TwoMeansTest::TwoMeansTest;

  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::Z;
  }
};

};  // namespace Inference

#endif
