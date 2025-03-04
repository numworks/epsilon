#ifndef INFERENCE_MODELS_STATISTIC_SLOPE_T_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_SLOPE_T_STATISTIC_H

#include <apps/shared/double_pair_store_preferences.h>
#include <apps/shared/linear_regression_store.h>
#include <poincare/statistics/inference.h>

#include "interval.h"
#include "table.h"
#include "table_from_store.h"
#include "test.h"

namespace Inference {

class SlopeTStatistic : public TableFromRegressionStore {
 public:
  SlopeTStatistic(Shared::GlobalContext* context)
      : TableFromRegressionStore(context) {
    m_series[0] = 0;
    initListsFromStorage();
  }
  int numberOfSeries() const override { return 1; }

 protected:
  void computeParametersFromSeries(const Statistic* stat,
                                   int pageIndex) override;

  double m_params[Poincare::Inference::NumberOfParameters(
      Poincare::Inference::TestType::Slope)];
};

class SlopeTInterval : public Interval, public SlopeTStatistic {
 public:
  using SlopeTStatistic::SlopeTStatistic;
  Table* table() override { return this; }
  void init() override { DoublePairStore::initListsFromStorage(); }
  void tidy() override { DoublePairStore::tidy(); }
  constexpr PcrInference::TestType testType() const override {
    return PcrInference::TestType::Slope;
  }
  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::T;
  }

  bool validateInputs(int pageIndex) override {
    return SlopeTStatistic::validateInputs(this, pageIndex);
  }

 private:
  // Slope
  constexpr static int k_numberOfExtraResults = 1;
  int numberOfExtraResults() const override { return k_numberOfExtraResults; }
  void extraResultAtIndex(int index, double* value, Poincare::Layout* message,
                          I18n::Message* subMessage, int* precision) override;

  double* parametersArray() override { return m_params; }
};

class SlopeTTest : public Test, public SlopeTStatistic {
 public:
  using SlopeTStatistic::SlopeTStatistic;
  Table* table() override { return this; }
  void init() override { DoublePairStore::initListsFromStorage(); }
  void tidy() override { DoublePairStore::tidy(); }

  constexpr PcrInference::TestType testType() const override {
    return PcrInference::TestType::Slope;
  }
  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::T;
  }

  bool validateInputs(int pageIndex) override {
    return SlopeTStatistic::validateInputs(this, pageIndex);
  }

 private:
  // Slope and SE
  constexpr static int k_numberOfExtraResults = 2;
  int numberOfExtraResults() const override { return k_numberOfExtraResults; }
  void extraResultAtIndex(int index, double* value, Poincare::Layout* message,
                          I18n::Message* subMessage, int* precision) override;

  double* parametersArray() override { return m_params; }
};

}  // namespace Inference

#endif
