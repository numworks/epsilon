#ifndef INFERENCE_MODELS_STATISTIC_RAW_DATA_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_RAW_DATA_STATISTIC_H

#include <apps/shared/double_pair_store_preferences.h>
#include <apps/shared/statistics_store.h>

#include "statistic.h"
#include "table.h"

namespace Inference {

enum class PageIndex : uint8_t { One, Two };

class RawDataStatistic : public Table, public Shared::StatisticsStore {
 public:
  constexpr static int k_numberOfDatasetOptions = 2;
  constexpr static int k_maxNumberOfSeries = 2;
  constexpr static int k_maxNumberOfColumns =
      k_maxNumberOfSeries * k_numberOfColumnsPerSeries;

  RawDataStatistic(Shared::GlobalContext* context)
      : StatisticsStore(context, &m_storePreferences),
        m_activePageIndex{PageIndex::One},
        m_series{-1, -1} {}

  int seriesAt(int index) const override {
    assert(index >= 0 && index < numberOfSeries() && index < m_series.size());
    return m_series[index];
  }
  void setSeriesAt(Statistic* stat, int index, int series) override;
  bool parametersAreValid(Statistic* stat);

  // DoublePairStore
  int seriesAtColumn(int column) const override {
    return seriesAt(static_cast<uint8_t>(m_activePageIndex));
  }

  void setActivePage(PageIndex pageIndex) { m_activePageIndex = pageIndex; }

  // Table
  void setParameterAtPosition(double value, int row, int column) override;
  double parameterAtPosition(int row, int column) const override;
  void deleteParametersInColumn(int column) override;
  bool deleteParameterAtPosition(int row, int column) override;
  void recomputeData() override;
  int maxNumberOfColumns() const override {
    return k_numberOfColumnsPerSeries * numberOfSeriesInTable();
  }
  int maxNumberOfRows() const override {
    return Shared::StatisticsStore::k_maxNumberOfPairs;
  }
  bool authorizedParameterAtPosition(double p, int row,
                                     int column) const override {
    return valueValidInColumn(p, column);
  }

 protected:
  virtual void syncParametersWithStore(const Statistic* stat) = 0;

  int numberOfResultsAndComputedParameters(const Statistic* stat,
                                           int results) const {
    return results + hasSeries() * stat->numberOfStatisticParameters();
  }
  bool computedParameterAtIndex(int* index, Statistic* stat, double* value,
                                Poincare::Layout* message,
                                I18n::Message* subMessage, int* precision);
  void initDatasetsIfSeries() {
    if (hasSeries()) {
      initDatasets();
    }
  }

  // Table
  Index2D initialDimensions() const override {
    return Index2D{.row = 1, .col = maxNumberOfColumns()};
  }

  /* In some cases (e.g. TwoMeans), the statistic can be displayed on several
   * pages. On each page, the selected series is displayed in a table. */
  PageIndex m_activePageIndex;

 private:
  Shared::DoublePairStorePreferences m_storePreferences;
  std::array<int, k_maxNumberOfSeries> m_series;
};

}  // namespace Inference

#endif
