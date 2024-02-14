#ifndef INFERENCE_MODELS_STATISTIC_SLOPE_T_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_SLOPE_T_STATISTIC_H

#include <apps/shared/double_pair_store_preferences.h>
#include <apps/shared/linear_regression_store.h>

#include "table.h"

namespace Inference {

class SlopeTStatistic : public Table, public Shared::LinearRegressionStore {
 public:
  SlopeTStatistic(Shared::GlobalContext* context)
      : Shared::LinearRegressionStore(context, &m_concreteStorePreferences),
        m_series(0) {
    initListsFromStorage();
  }

  I18n::Message title() const {
    return I18n::Message::HypothesisControllerTitleSlope;
  }
  int series() const { return m_series; }
  void setSeries(int series) { m_series = series; }

  // Table
  void setParameterAtPosition(double value, int row, int column) override {
    set(value, m_series, column, row, false);
  }
  double parameterAtPosition(int row, int column) const override;
  bool deleteParameterAtPosition(int row, int column) override;
  void recomputeData() override { updateSeries(m_series); }
  int maxNumberOfColumns() const override { return k_maxNumberOfColumns; }
  int maxNumberOfRows() const override { return k_maxNumberOfPairs; }

  constexpr static int k_maxNumberOfColumns = 2;

 protected:
  int numberOfTableParameters() const {
    return k_maxNumberOfPairs * k_maxNumberOfColumns;
  }
  bool authorizedParameterAtIndex(double p, int i) const;
  double computeStandardError() const;
  bool validateInputs() {
    return seriesIsValid(m_series) && numberOfPairsOfSeries(m_series) > 2;
  }

 private:
  // Table
  Index2D initialDimensions() const override {
    return Index2D{.row = 1, .col = 2};
  }

  Shared::DoublePairStorePreferences m_concreteStorePreferences;
  int m_series;
};

}  // namespace Inference

#endif
