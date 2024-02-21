#ifndef INFERENCE_MODELS_STATISTIC_ONE_MEAN_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_ONE_MEAN_STATISTIC_H

#include <apps/shared/double_pair_store_preferences.h>
#include <apps/shared/statistics_store.h>

#include "interfaces/significance_tests.h"
#include "statistic.h"
#include "table.h"

namespace Inference {

class OneMeanStatistic : public Table, public Shared::StatisticsStore {
 public:
  constexpr static int k_numberOfDatasetOptions = 2;
  constexpr static int k_maxNumberOfColumns = 2;

  OneMeanStatistic(Shared::GlobalContext* context)
      : StatisticsStore(context, &m_storePreferences), m_series(-1) {}

  int series() const { return m_series; }
  void setSeries(int series, Statistic* state);
  bool parametersAreValid(Statistic* stat);

  // Table
  void setParameterAtPosition(double value, int row, int column) override;
  double parameterAtPosition(int row, int column) const override;
  void deleteParametersInColumn(int column) override;
  bool deleteParameterAtPosition(int row, int column) override;
  void recomputeData() override { updateSeries(m_series); }
  int maxNumberOfColumns() const override { return k_maxNumberOfColumns; }
  int maxNumberOfRows() const override {
    return Shared::StatisticsStore::k_maxNumberOfPairs;
  }
  bool authorizedParameterAtPosition(double p, int row,
                                     int column) const override {
    return valueValidInColumn(p, column);
  }

  // DoublePairStore
  int seriesAtColumn(int column) const override { return m_series; }

 protected:
  static OneMean::Type OneMeanType(const Statistic* s) {
    if (s->distributionType() == DistributionType::T) {
      return OneMean::Type::T;
    }
    assert(s->distributionType() == DistributionType::Z);
    return OneMean::Type::Z;
  }

  void syncParametersWithStore();
  int numberOfResultsAndComputedParameters(int results) const {
    return results + (m_series >= 0) * OneMean::k_numberOfParams;
  }
  bool resultOrComputedParameterAtIndex(int* index, Statistic* stat,
                                        double* value,
                                        Poincare::Layout* message,
                                        I18n::Message* subMessage,
                                        int* precision);
  void initDatasetsIfSeries() {
    if (m_series >= 0) {
      initDatasets();
    }
  }

  // Table
  Index2D initialDimensions() const override {
    return Index2D{.row = 1, .col = 2};
  }

  Shared::DoublePairStorePreferences m_storePreferences;
  double m_params[OneMean::k_numberOfParams];
  int m_series;
};

}  // namespace Inference

#endif
