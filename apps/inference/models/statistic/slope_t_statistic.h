#ifndef INFERENCE_MODELS_STATISTIC_SLOPE_T_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_SLOPE_T_STATISTIC_H

#include <apps/shared/double_pair_store_preferences.h>
#include <apps/shared/linear_regression_store.h>

#include "table.h"

namespace Inference {

class SlopeTStatistic : public Table, public Shared::LinearRegressionStore {
 public:
  constexpr static int k_maxNumberOfColumns = 2;

  SlopeTStatistic(Shared::GlobalContext* context)
      : Shared::LinearRegressionStore(context, &m_concreteStorePreferences),
        m_series(0) {
    initListsFromStorage();
  }

  I18n::Message title() const {
    return I18n::Message::HypothesisControllerTitleSlope;
  }
  int numberOfSeries() const override { return 1; }
  int seriesAt(int) const override { return m_series; }
  void setSeriesAt(Statistic*, int index, int series) override {
    m_series = series;
  }

  // DoublePairStore
  int seriesAtColumn(int column) const override { return m_series; }

  // Table
  void setParameterAtPosition(double value, int row, int column) override {
    set(value, m_series, column, row, false);
  }
  double parameterAtPosition(int row, int column) const override;
  bool deleteParameterAtPosition(int row, int column) override;
  void recomputeData() override { updateSeries(m_series); }
  int maxNumberOfColumns() const override { return k_maxNumberOfColumns; }
  int maxNumberOfRows() const override { return k_maxNumberOfPairs; }

 protected:
  int numberOfTableParameters() const {
    return k_maxNumberOfPairs * k_maxNumberOfColumns;
  }
  bool authorizedParameterAtIndex(double p, int i) const;
  double computeStandardError() const;
  bool validateInputs(int pageIndex) { return validateSeries(this, pageIndex); }

  // Table
  Index2D initialDimensions() const override {
    return Index2D{.row = 1, .col = 2};
  }

  Shared::DoublePairStorePreferences m_concreteStorePreferences;
  int m_series;
};

}  // namespace Inference

#endif
