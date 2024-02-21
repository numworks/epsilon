#ifndef INFERENCE_MODELS_STATISTIC_RAW_DATA_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_RAW_DATA_STATISTIC_H

#include <apps/shared/double_pair_store_preferences.h>
#include <apps/shared/statistics_store.h>

#include "statistic.h"
#include "table.h"

namespace Inference {

class RawDataStatistic : public Table, public Shared::StatisticsStore {
 public:
  constexpr static int k_numberOfDatasetOptions = 2;
  constexpr static int k_maxNumberOfSeries = 2;
  constexpr static int k_maxNumberOfColumns =
      k_maxNumberOfSeries * k_numberOfColumnsPerSeries;

  RawDataStatistic(Shared::GlobalContext* context)
      : StatisticsStore(context, &m_storePreferences), m_series{-1, -1} {}

  virtual int numberOfSeries() const = 0;

  bool hasSeries() const;
  int seriesAt(int index) const {
    assert(index < numberOfSeries() && index < k_maxNumberOfSeries);
    return m_series[index];
  }
  void setSeriesAt(Statistic* stat, int index, int series);
  bool parametersAreValid(Statistic* stat);

  // DoublePairStore
  int seriesAtColumn(int column) const override {
    return seriesAt(column / k_numberOfColumnsPerSeries);
  }

  // Table
  void setParameterAtPosition(double value, int row, int column) override;
  double parameterAtPosition(int row, int column) const override;
  void deleteParametersInColumn(int column) override;
  bool deleteParameterAtPosition(int row, int column) override;
  void recomputeData() override;
  int maxNumberOfColumns() const override {
    return k_numberOfColumnsPerSeries * numberOfSeries();
  }
  int maxNumberOfRows() const override {
    return Shared::StatisticsStore::k_maxNumberOfPairs;
  }
  bool authorizedParameterAtPosition(double p, int row,
                                     int column) const override {
    return valueValidInColumn(p, column);
  }

 protected:
  virtual void syncParametersWithStore(Statistic* stat) = 0;

  int numberOfResultsAndComputedParameters(const Statistic* stat,
                                           int results) const {
    return results + hasSeries() * stat->numberOfStatisticParameters();
  }
  bool resultOrComputedParameterAtIndex(int* index, Statistic* stat,
                                        double* value,
                                        Poincare::Layout* message,
                                        I18n::Message* subMessage,
                                        int* precision);
  void initDatasetsIfSeries() {
    if (hasSeries()) {
      initDatasets();
    }
  }

  // Table
  Index2D initialDimensions() const override {
    return Index2D{.row = 1, .col = maxNumberOfColumns()};
  }

 private:
  Shared::DoublePairStorePreferences m_storePreferences;
  int m_series[k_maxNumberOfSeries];
};

}  // namespace Inference

#endif
