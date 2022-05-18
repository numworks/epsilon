#ifndef PROBABILITY_MODELS_STATISTIC_SLOPE_T_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_SLOPE_T_STATISTIC_H

#include <apps/shared/double_pair_store.h>
#include "table.h"

namespace Inference {

class SlopeTStatistic : public Table, public Shared::DoublePairStore {
public:
  SlopeTStatistic(Shared::GlobalContext * context) : Shared::DoublePairStore(context) {
    initListsFromStorage();
  }
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
private:
  // Table
  Index2D initialDimensions() const override { return Index2D{.row = 1, .col = 2}; }
};

}  // namespace Inference

#endif /* PROBABILITY_MODELS_STATISTIC_ONE_MEAN_T_TEST_H */
