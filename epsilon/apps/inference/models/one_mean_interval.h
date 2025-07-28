#ifndef INFERENCE_MODELS_ONE_MEAN_INTERVAL_H
#define INFERENCE_MODELS_ONE_MEAN_INTERVAL_H

#include <poincare/statistics/inference.h>

#include "confidence_interval.h"
#include "inference/models/input_table_from_store.h"
#include "one_mean_statistic.h"

namespace Inference {

class OneMeanInterval : public ConfidenceInterval, public OneMeanStatistic {
 public:
  using OneMeanStatistic::OneMeanStatistic;
  InputTable* table() override { return this; }
  void init() override { initDatasetsIfSeries(); }
  void tidy() override { tidyDatasets(); }

  constexpr TestType testType() const override { return TestType::OneMean; }

  double preProcessParameter(double p, int index) const override {
    return preProcessOneMeanParameter(p, index);
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

class OneMeanTInterval : public OneMeanInterval {
 public:
  using OneMeanInterval::OneMeanInterval;
  constexpr StatisticType statisticType() const override {
    return StatisticType::T;
  }
};

class OneMeanZInterval : public OneMeanInterval {
 public:
  using OneMeanInterval::OneMeanInterval;
  constexpr StatisticType statisticType() const override {
    return StatisticType::Z;
  }
};

}  // namespace Inference

#endif
