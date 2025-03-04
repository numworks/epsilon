#ifndef INFERENCE_MODELS_STATISTIC_TWO_MEANS_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_TWO_MEANS_INTERVAL_H

#include "interval.h"
#include "two_means_statistic.h"

namespace Inference {

class TwoMeansInterval : public Interval, public TwoMeansStatistic {
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

class TwoMeansTInterval : public TwoMeansInterval {
 public:
  using TwoMeansInterval::TwoMeansInterval;

  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::T;
  }
};

class PooledTwoMeansTInterval : public TwoMeansInterval {
 public:
  using TwoMeansInterval::TwoMeansInterval;

  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::TPooled;
  }
};

class TwoMeansZInterval : public TwoMeansInterval {
 public:
  using TwoMeansInterval::TwoMeansInterval;

  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::Z;
  }
};

}  // namespace Inference

#endif
