#ifndef INFERENCE_MODELS_TWO_MEANS_TEST_H
#define INFERENCE_MODELS_TWO_MEANS_TEST_H

#include "inference/models/input_table_from_store.h"
#include "significance_test.h"
#include "two_means_statistic.h"

namespace Inference {

class TwoMeansTest : public SignificanceTest, public TwoMeansStatistic {
 public:
  using TwoMeansStatistic::TwoMeansStatistic;
  InputTable* table() override { return this; }
  void init() override { initDatasetsIfSeries(); }
  void tidy() override { tidyDatasets(); }

  constexpr TestType testType() const override { return TestType::TwoMeans; }

  double preProcessParameter(double p, int index) const override {
    return preProcessTwoMeansParameter(p, index);
  }

  bool validateInputs(int pageIndex) override {
    return InputTableFromStatisticStore::validateInputs(this, pageIndex);
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

  constexpr StatisticType statisticType() const override {
    return StatisticType::T;
  }
};

class PooledTwoMeansTTest : public TwoMeansTest {
 public:
  using TwoMeansTest::TwoMeansTest;

  constexpr StatisticType statisticType() const override {
    return StatisticType::TPooled;
  }
};

class TwoMeansZTest : public TwoMeansTest {
 public:
  using TwoMeansTest::TwoMeansTest;

  constexpr StatisticType statisticType() const override {
    return StatisticType::Z;
  }
};

};  // namespace Inference

#endif
