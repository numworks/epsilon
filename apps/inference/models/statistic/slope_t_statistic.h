#ifndef INFERENCE_MODELS_STATISTIC_SLOPE_T_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_SLOPE_T_STATISTIC_H

#include <apps/shared/linear_regression_store.h>
#include "interfaces/significance_tests.h"
#include "table.h"

namespace Inference {

class SlopeTStatistic : public Table, public Shared::LinearRegressionStore {
public:
  SlopeTStatistic(Shared::GlobalContext * context) : Shared::LinearRegressionStore(context) {
    initListsFromStorage();
  }

  I18n::Message title() const { return I18n::Message::HypothesisControllerTitleSlope; }

  // Table
  void setParameterAtPosition(double value, int row, int column) override { set(value, 0, column, row, false); }
  double parameterAtPosition(int row, int column) const override;
  void recomputeData() override { updateSeries(0); }
  int maxNumberOfColumns() const override { return k_maxNumberOfColumns; }
  int maxNumberOfRows() const override { return k_maxNumberOfPairs; }

  constexpr static int k_maxNumberOfColumns = 2;
protected:
  int numberOfTableParameters() const { return k_maxNumberOfPairs * k_maxNumberOfColumns; }
  bool authorizedParameterAtIndex(double p, int i) const;
  double computeStandardError() const;
  bool validateInputs() { return seriesIsValid(0) && numberOfPairsOfSeries(0) > 2; }
private:
  // Table
  Index2D initialDimensions() const override { return Index2D{.row = 1, .col = 2}; }
};

}  // namespace Inference

#endif /* INFERENCE_MODELS_STATISTIC_ONE_MEAN_T_TEST_H */
