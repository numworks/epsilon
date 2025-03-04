#ifndef INFERENCE_MODELS_STATISTIC_RAW_DATA_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_RAW_DATA_STATISTIC_H

#include <apps/shared/double_pair_store_preferences.h>
#include <apps/shared/linear_regression_store.h>
#include <apps/shared/statistics_store.h>

#include "shared/double_pair_store.h"
#include "statistic.h"
#include "table.h"

namespace Inference {

class TableFromStore : public Table {
 public:
  constexpr static int k_numberOfColumnsPerSeries =
      Shared::DoublePairStore::k_numberOfColumnsPerSeries;
  constexpr static int k_numberOfDatasetOptions = 2;
  constexpr static int k_maxNumberOfSeries = 2;
  constexpr static int k_maxNumberOfColumns =
      k_maxNumberOfSeries * k_numberOfColumnsPerSeries;

  TableFromStore() : m_series{-1, -1}, m_activePageIndex{0} {}

  int seriesAt(int pageIndex) const override {
    assert(pageIndex >= 0 && pageIndex < numberOfSeries() &&
           numberOfSeries() <= m_series.size());
    return m_series[pageIndex];
  }
  void setSeriesAt(Statistic* stat, int pageIndex, int series) override;
  bool validateInputs(Statistic* stat, int pageIndex);
  bool authorizedValueAtPosition(double p, int row, int column) const override;

  void setActivePage(uint8_t pageIndex) { m_activePageIndex = pageIndex; }

  // Table
  void setValueAtPosition(double value, int row, int column) override;
  double valueAtPosition(int row, int column) const override;
  bool deleteValueAtPosition(int row, int column) override;
  void recomputeData() override;
  int maxNumberOfColumns() const override { return k_numberOfColumnsPerSeries; }
  int maxNumberOfRows() const override {
    return Shared::StatisticsStore::k_maxNumberOfPairs;
  }

 protected:
  virtual void computeParametersFromSeries(const Statistic* stat,
                                           int pageIndex) = 0;

  int numberOfComputedParameters(const Statistic* stat) const {
    return static_cast<int>(hasAllSeries()) * stat->numberOfTestParameters();
  }

  // Table
  Index2D initialDimensions() const override {
    return Index2D{.row = 1, .col = maxNumberOfColumns()};
  }

  virtual Shared::DoublePairStore* doublePairStore() = 0;

  Shared::DoublePairStorePreferences m_dblePairStorePreferences;
  std::array<int, k_maxNumberOfSeries> m_series;
  /* In some cases (e.g. TwoMeans), the statistic can be displayed on several
   * pages. On each page, the selected series is displayed in a table. */

  uint8_t m_activePageIndex;
};

class TableFromStatisticStore : public TableFromStore,
                                public Shared::StatisticsStore {
 public:
  TableFromStatisticStore(Shared::GlobalContext* context)
      : TableFromStore(),
        Shared::StatisticsStore(context, &m_dblePairStorePreferences) {}

  // DoublePairStore
  int seriesAtColumn(int column) const override {
    return seriesAt(m_activePageIndex);
  }

  void setSeriesAt(Statistic* stat, int pageIndex, int series) override;
  void deleteValuesInColumn(int column) override;

 protected:
  void initDatasetsIfSeries() {
    if (hasSeries(m_activePageIndex)) {
      initDatasets();
    }
  }

  bool computedParameterAtIndex(int index, Statistic* stat, double* value,
                                Poincare::Layout* message,
                                I18n::Message* subMessage, int* precision);

  Shared::DoublePairStore* doublePairStore() override { return this; }
};

class TableFromRegressionStore : public TableFromStore,
                                 public Shared::LinearRegressionStore {
 public:
  TableFromRegressionStore(Shared::GlobalContext* context)
      : TableFromStore(),
        Shared::LinearRegressionStore(context, &m_dblePairStorePreferences) {}

  // DoublePairStore
  int seriesAtColumn(int column) const override {
    return seriesAt(m_activePageIndex);
  }

 protected:
  Shared::DoublePairStore* doublePairStore() override { return this; }
};

}  // namespace Inference

#endif
