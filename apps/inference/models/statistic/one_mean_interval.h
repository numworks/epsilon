#ifndef INFERENCE_MODELS_STATISTIC_ONE_MEAN_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_ONE_MEAN_INTERVAL_H

#include <poincare/statistics/inference.h>

#include "inference/models/statistic/table_from_store.h"
#include "interval.h"
#include "one_mean_statistic.h"

namespace Inference {

class OneMeanInterval : public Interval, public OneMeanStatistic {
 public:
  using OneMeanStatistic::OneMeanStatistic;
  Table* table() override { return this; }
  void init() override { initDatasetsIfSeries(); }
  void tidy() override { tidyDatasets(); }

  constexpr PcrInference::TestType testType() const override {
    return PcrInference::TestType::OneMean;
  }

  double preProcessParameter(double p, int index) const override {
    return preProcessOneMeanParameter(p, index);
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

class OneMeanTInterval : public OneMeanInterval {
 public:
  using OneMeanInterval::OneMeanInterval;
  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::T;
  }
};

class OneMeanZInterval : public OneMeanInterval {
 public:
  using OneMeanInterval::OneMeanInterval;
  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::Z;
  }
};

}  // namespace Inference

#endif
