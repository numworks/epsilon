#include "median_regression.h"

#include <assert.h>
#include <omg/list.h>

namespace Poincare::Regression {

double MedianRegression::getMedianValue(const Series* series,
                                        uint8_t* sortedIndex, int column,
                                        int startIndex, int endIndex) const {
  // TODO: this should be factorized with other median code
  assert(endIndex != startIndex);
  int medianIndex = startIndex + (endIndex - startIndex) / 2;
  double upperMedian = series->get(column, sortedIndex[medianIndex]);
  if ((endIndex - startIndex) % 2 == 1) {
    return upperMedian;
  }
  double lowerMedian = series->get(column, sortedIndex[medianIndex - 1]);
  return (lowerMedian + upperMedian) / 2;
}

static void sortIndexByColumn(uint8_t* sortedIndex, const Series* series,
                              int column, int startIndex, int endIndex) {
  assert(startIndex < endIndex);
  void* pack[] = {const_cast<Series*>(series), sortedIndex + startIndex,
                  &column};
  OMG::List::Sort(
      [](int i, int j, void* ctx, int n) {  // Swap method
        void** pack = reinterpret_cast<void**>(ctx);
        uint8_t* sortedIndex = reinterpret_cast<uint8_t*>(pack[1]);
        std::swap(sortedIndex[i], sortedIndex[j]);
      },
      [](int i, int j, void* ctx, int n) {  // Comparison method
        void** pack = reinterpret_cast<void**>(ctx);
        const Series* series = reinterpret_cast<const Series*>(pack[0]);
        uint8_t* sortedIndex = reinterpret_cast<uint8_t*>(pack[1]);
        int column = *reinterpret_cast<int*>(pack[2]);
        return series->get(column, sortedIndex[i]) >=
               series->get(column, sortedIndex[j]);
      },
      pack, endIndex - startIndex);
}

Regression::Coefficients MedianRegression::privateFit(
    const Series* series, Poincare::Context* context) const {
  uint8_t numberOfDots = series->numberOfPairs();
  assert(slopeCoefficientIndex() == 0 && yInterceptCoefficientIndex() == 1);
  if (numberOfDots < 3) {
    return {NAN, NAN};
  }

  uint8_t sortedIndex[Regression::k_maxNumberOfPairs];
  for (uint8_t i = 0; i < numberOfDots; i++) {
    sortedIndex[i] = i;
  }
  sortIndexByColumn(sortedIndex, series, 0, 0, numberOfDots);

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
    return {NAN, NAN};
  }

  sortIndexByColumn(sortedIndex, series, 1, 0, sizeOfRightLeftGroup);
  sortIndexByColumn(sortedIndex, series, 1, sizeOfRightLeftGroup,
                    sizeOfRightLeftGroup + sizeOfMiddleGroup);
  sortIndexByColumn(sortedIndex, series, 1,
                    sizeOfRightLeftGroup + sizeOfMiddleGroup, numberOfDots);

  leftPoint[1] =
      getMedianValue(series, sortedIndex, 1, 0, sizeOfRightLeftGroup);
  middlePoint[1] = getMedianValue(series, sortedIndex, 1, sizeOfRightLeftGroup,
                                  sizeOfRightLeftGroup + sizeOfMiddleGroup);
  rightPoint[1] =
      getMedianValue(series, sortedIndex, 1,
                     sizeOfRightLeftGroup + sizeOfMiddleGroup, numberOfDots);

  double a = (rightPoint[1] - leftPoint[1]) / (rightPoint[0] - leftPoint[0]);
  return {a, ((leftPoint[1] - a * leftPoint[0]) +
              (middlePoint[1] - a * middlePoint[0]) +
              (rightPoint[1] - a * rightPoint[0])) /
                 3};
}

}  // namespace Poincare::Regression
