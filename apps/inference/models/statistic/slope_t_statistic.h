#ifndef PROBABILITY_MODELS_STATISTIC_SLOPE_T_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_SLOPE_T_STATISTIC_H

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
  double parameterAtPosition(int row, int column) const override {
    if (row >= numberOfPairsOfSeries(0)) {
      return NAN;
    }
    return get(0, column, row);
  }
  void recomputeData() override { updateSeries(0); }
  int maxNumberOfColumns() const override { return k_maxNumberOfColumns; }
  int maxNumberOfRows() const override { return k_maxNumberOfPairs; }

  constexpr static int k_maxNumberOfColumns = 2;
protected:
  int numberOfTableParameters() const { return numberOfPairsOfSeries(0) * k_maxNumberOfColumns; }
  bool authorizedParameterAtIndex(double p, int i) const {
    assert(i == numberOfTableParameters());
    return SignificanceTest::ValidThreshold(p);
  }
private:
  // Table
  Index2D initialDimensions() const override { return Index2D{.row = 1, .col = 2}; }
};

}  // namespace Inference

#endif /* PROBABILITY_MODELS_STATISTIC_ONE_MEAN_T_TEST_H */
