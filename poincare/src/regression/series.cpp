#include "series.h"

#include "dataset_adapter.h"

namespace Poincare::Regression {

bool Series::numberOfDistinctAbscissaeGreaterOrEqualTo(int i) const {
  int count = 0;
  for (int j = 0; j < numberOfPairs(); j++) {
    if (count >= i) {
      return true;
    }
    double currentAbscissa = getX(j);
    bool firstOccurrence = true;
    for (int k = 0; k < j; k++) {
      if (getX(k) == currentAbscissa) {
        firstOccurrence = false;
        break;
      }
    }
    if (firstOccurrence) {
      count++;
    }
  }
  return count >= i;
}

StatisticsDatasetFromSeriesColumn Series::createDatasetFromColumn(
    int i, StatisticsCalculationOptions options) const {
  return StatisticsDatasetFromSeriesColumn(this, i, options.lnOfValues(i),
                                           options.oppositeOfValues(i));
}

double Series::columnProductSum(StatisticsCalculationOptions options) const {
  double result = 0;
  for (int k = 0; k < numberOfPairs(); k++) {
    double value0 = options.transformValue(getX(k), 0);
    double value1 = options.transformValue(getY(k), 1);
    result += value0 * value1;
  }
  return result;
}

double Series::covariance(StatisticsCalculationOptions options) const {
  double mean0 = meanOfColumn(0, options);
  double mean1 = meanOfColumn(1, options);
  return columnProductSum(options) / numberOfPairs() - mean0 * mean1;
}

double Series::slope(StatisticsCalculationOptions options) const {
  return covariance(options) / varianceOfColumn(0, options);
}

double Series::yIntercept(StatisticsCalculationOptions options) const {
  double meanOfX = meanOfColumn(0, options);
  double meanOfY = meanOfColumn(1, options);
  return meanOfY - slope(options) * meanOfX;
}

double Series::meanOfColumn(int i, StatisticsCalculationOptions options) const {
  return createDatasetFromColumn(i, options).mean();
}

double Series::varianceOfColumn(int i,
                                StatisticsCalculationOptions options) const {
  return createDatasetFromColumn(i, options).variance();
}

}  // namespace Poincare::Regression
