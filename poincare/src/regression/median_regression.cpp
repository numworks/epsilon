#include "median_regression.h"

#include <assert.h>

namespace Poincare::Regression {

double MedianRegression::getMedianValue(const Series* series,
                                        uint8_t* sortedIndex, int column,
                                        int startIndex, int endIndex) const {
  // TODO: this should be factorized with other median code
  assert(endIndex != startIndex);
  int medianIndex = sortedIndex[startIndex + (endIndex - startIndex) / 2];
  double upperMedian =
      column == 0 ? series->getX(medianIndex) : series->getY(medianIndex);
  if ((endIndex - startIndex) % 2 == 1) {
    return upperMedian;
  }
  double lowerMedian = column == 0 ? series->getX(medianIndex - 1)
                                   : series->getY(medianIndex - 1);
  return (lowerMedian + upperMedian) / 2;
}

void MedianRegression::privateFit(const Series* series,
                                  double* modelCoefficients,
                                  Poincare::Context* context) const {
  uint8_t numberOfDots = series->numberOfPairs();
  assert(slopeCoefficientIndex() == 0 && yInterceptCoefficientIndex() == 1);
  if (numberOfDots < 3) {
    modelCoefficients[0] = NAN;
    modelCoefficients[1] = NAN;
    return;
  }

  uint8_t sortedIndex[Store::k_maxNumberOfPairs];
  for (uint8_t i = 0; i < numberOfDots; i++) {
    sortedIndex[i] = i;
  }
  store->sortIndexByColumn(sortedIndex, series, 0, 0, numberOfDots);

  int sizeOfMiddleGroup = numberOfDots / 3 + (numberOfDots % 3 == 1 ? 1 : 0);
  int sizeOfRightLeftGroup = numberOfDots / 3 + (numberOfDots % 3 == 2 ? 1 : 0);

  double leftPoint[2];
  double middlePoint[2];
  double rightPoint[2];

  leftPoint[0] =
      getMedianValue(series, sortedIndex, 0, 0, sizeOfRightLeftGroup);
  middlePoint[0] = getMedianValue(series, sortedIndex, 0, sizeOfRightLeftGroup,
                                  sizeOfRightLeftGroup + sizeOfMiddleGroup);
  rightPoint[0] =
      getMedianValue(series, sortedIndex, 0,
                     sizeOfRightLeftGroup + sizeOfMiddleGroup, numberOfDots);

  if (rightPoint[0] == leftPoint[0]) {
    modelCoefficients[0] = NAN;
    modelCoefficients[1] = NAN;
    return;
  }

  store->sortIndexByColumn(sortedIndex, series, 1, 0, sizeOfRightLeftGroup);
  store->sortIndexByColumn(sortedIndex, series, 1, sizeOfRightLeftGroup,
                           sizeOfRightLeftGroup + sizeOfMiddleGroup);
  store->sortIndexByColumn(sortedIndex, series, 1,
                           sizeOfRightLeftGroup + sizeOfMiddleGroup,
                           numberOfDots);

  leftPoint[1] =
      getMedianValue(series, sortedIndex, 1, 0, sizeOfRightLeftGroup);
  middlePoint[1] = getMedianValue(series, sortedIndex, 1, sizeOfRightLeftGroup,
                                  sizeOfRightLeftGroup + sizeOfMiddleGroup);
  rightPoint[1] =
      getMedianValue(series, sortedIndex, 1,
                     sizeOfRightLeftGroup + sizeOfMiddleGroup, numberOfDots);

  double a = (rightPoint[1] - leftPoint[1]) / (rightPoint[0] - leftPoint[0]);
  modelCoefficients[0] = a;
  modelCoefficients[1] = ((leftPoint[1] - a * leftPoint[0]) +
                          (middlePoint[1] - a * middlePoint[0]) +
                          (rightPoint[1] - a * rightPoint[0])) /
                         3;
}

}  // namespace Poincare::Regression
