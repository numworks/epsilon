#include "chi2_test.h"

#include "poincare/src/statistics/data_table.h"

namespace Poincare::Internal::Inference::SignificanceTest::Chi2 {

bool IsObservedValueValid(double value) {
  return !std::isnan(value) && value >= 0.;
}

bool IsExpectedValueValid(double value) {
  // Expected values can be null
  return IsObservedValueValid(value) && value >= DBL_EPSILON;
}

bool AreHomogeneityInputsValid(const DataTable* observedValues) {
  int maxRow = observedValues->numberOfRows();
  int maxCol = observedValues->numberOfColumns();

  if (maxCol <= 1 || maxRow <= 1) {
    return false;
  }

  /* - No value should be undef or negative
   * - Neither a whole row nor a whole column should be null.
   * - The whole population should not be null. */
  double total = 0.;
  for (int row = 0; row < maxRow; row++) {
    bool rowIsNull = true;
    for (int col = 0; col < maxCol; col++) {
      double value = observedValues->get(col, row);
      total += value;
      if (!IsObservedValueValid(value)) {
        return false;
      }
      bool valIsNull = std::fabs(value) < DBL_MIN;

      if (row == 0 && valIsNull) {
        // Check if the whole column is null
        bool colIsNull = true;
        for (int i = 1; i < maxRow; i++) {
          colIsNull =
              colIsNull && std::fabs(observedValues->get(col, i)) < DBL_MIN;
          if (!colIsNull) {
            break;
          }
        }
        if (colIsNull) {
          return false;
        }
      }

      rowIsNull = rowIsNull && valIsNull;
    }
    if (rowIsNull) {
      return false;
    }
  }

  return std::fabs(total) >= DBL_MIN;
}

bool AreGoodnessInputsValid(const DataTable* observedValues,
                            const DataTable* expectedValues) {
  int maxRow = observedValues->numberOfRows();
  int maxCol = observedValues->numberOfColumns();

  if (maxCol != 1 || maxCol != expectedValues->numberOfColumns() ||
      maxRow < 1 || maxRow > expectedValues->numberOfRows()) {
    return false;
  }
  for (int row = 0; row < maxRow; row++) {
    double observed = observedValues->get(0, row);
    double expected = expectedValues->get(0, row);
    if (!IsObservedValueValid(observed) || !IsExpectedValueValid(expected)) {
      return false;
    }
  }
  return true;
}

void FillHomogeneityExpectedValues(const DataTable* observedValues,
                                   DataTable* expectedValues) {
  int maxRow = observedValues->numberOfRows();
  int maxCol = observedValues->numberOfColumns();

  assert(expectedValues->numberOfColumns() >= maxCol);
  assert(expectedValues->numberOfRows() >= maxRow);

  double total = 0;
  for (int col = 0; col < maxCol; col++) {
    double colTotal = observedValues->columnSum(col);
    total += colTotal;
    // Store the column total in the last row to avoid recomputing it later
    expectedValues->set(colTotal, col, maxRow - 1);
  }

  for (int row = 0; row < maxRow; row++) {
    double rowTotal = observedValues->rowSum(row);
    for (int col = 0; col < maxCol; col++) {
      double colTotal = expectedValues->get(col, maxRow - 1);
      // Note : Divide before multiplying to avoid some cases of double
      // overflow
      double expected = (colTotal / total) * rowTotal;
      expectedValues->set(expected, col, row);
    }
  }
}

bool IsDegreesOfFreedomValid(double p) {
  return p == std::round(p) && p >= 1.0;
}

int ComputeHomogeneityDegreesOfFreedom(const DataTable* observedValues) {
  int maxRow = observedValues->numberOfRows();
  int maxCol = observedValues->numberOfColumns();
  return (maxRow - 1) * (maxCol - 1);
}

int ComputeGoodnessOfFitDegreesOfFreedom(const DataTable* observedValues) {
  return observedValues->numberOfRows() - 1;
}

void FillContributions(const DataTable* observedValues,
                       const DataTable* expectedValues,
                       DataTable* contributions) {
  int maxRow = observedValues->numberOfRows();
  int maxCol = observedValues->numberOfColumns();

  assert(expectedValues->numberOfColumns() >= maxCol);
  assert(expectedValues->numberOfRows() >= maxRow);
  assert(contributions->numberOfColumns() >= maxCol);
  assert(contributions->numberOfRows() >= maxRow);

  for (int row = 0; row < maxRow; row++) {
    for (int col = 0; col < maxCol; col++) {
      double observed = observedValues->get(col, row);
      double expected = expectedValues->get(col, row);
      contributions->set(std::pow(observed - expected, 2.) / expected, col,
                         row);
    }
  }
}

double ComputeCriticalValue(const DataTable* contributions) {
  double chi2 = 0;
  for (int row = 0; row < contributions->numberOfRows(); row++) {
    for (int col = 0; col < contributions->numberOfColumns(); col++) {
      chi2 += contributions->get(col, row);
    }
  }
  return chi2;
}

}  // namespace Poincare::Internal::Inference::SignificanceTest::Chi2
