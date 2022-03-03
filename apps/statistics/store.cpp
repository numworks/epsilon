#include "store.h"
#include <apps/global_preferences.h>
#include <poincare/normal_distribution.h>
#include <assert.h>
#include <float.h>
#include <cmath>
#include <string.h>
#include <ion.h>
#include <limits.h>

using namespace Shared;

namespace Statistics {

static_assert(Store::k_numberOfSeries == 3, "The constructor of Statistics::Store should be changed");

Store::Store() :
  MemoizedCurveViewRange(),
  DoublePairStore(),
  m_barWidth(1.0),
  m_firstDrawnBarAbscissa(0.0)
{
}

uint32_t Store::barChecksum() const {
  double data[2] = {m_barWidth, m_firstDrawnBarAbscissa};
  size_t dataLengthInBytes = 2*sizeof(double);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  return Ion::crc32Word((uint32_t *)data, dataLengthInBytes/sizeof(uint32_t));
}

/* Histogram bars */

void Store::setBarWidth(double barWidth) {
  assert(barWidth > 0.0);
  m_barWidth = barWidth;
}

double Store::heightOfBarAtIndex(int series, int index) const {
  return sumOfValuesBetween(series, startOfBarAtIndex(series, index), endOfBarAtIndex(series, index));
}

double Store::heightOfBarAtValue(int series, double value) const {
  double width = barWidth();
  int barNumber = std::floor((value - m_firstDrawnBarAbscissa)/width);
  double lowerBound = m_firstDrawnBarAbscissa + ((double)barNumber)*width;
  double upperBound = m_firstDrawnBarAbscissa + ((double)(barNumber+1))*width;
  return sumOfValuesBetween(series, lowerBound, upperBound);
}

double Store::startOfBarAtIndex(int series, int index) const {
  double firstBarAbscissa = m_firstDrawnBarAbscissa + m_barWidth*std::floor((minValue(series)- m_firstDrawnBarAbscissa)/m_barWidth);
  return firstBarAbscissa + index * m_barWidth;
}

double Store::endOfBarAtIndex(int series, int index) const {
  return startOfBarAtIndex(series, index+1);
}

double Store::numberOfBars(int series) const {
  double firstBarAbscissa = m_firstDrawnBarAbscissa + m_barWidth*std::floor((minValue(series)- m_firstDrawnBarAbscissa)/m_barWidth);
  return std::ceil((maxValue(series) - firstBarAbscissa)/m_barWidth)+1;
}

bool Store::scrollToSelectedBarIndex(int series, int index) {
  float startSelectedBar = startOfBarAtIndex(series, index);
  float windowRange = xMax() - xMin();
  float range = windowRange/(1+k_displayLeftMarginRatio+k_displayRightMarginRatio);
  if (xMin() + k_displayLeftMarginRatio*range > startSelectedBar) {
    // Only update the grid unit when setting xMax
    setHistogramXMin(startSelectedBar - k_displayLeftMarginRatio*range, false);
    setHistogramXMax(xMin() + windowRange, true);
    return true;
  }
  float endSelectedBar = endOfBarAtIndex(series, index);
  if (endSelectedBar > xMax() - k_displayRightMarginRatio*range) {
    setHistogramXMax(endSelectedBar + k_displayRightMarginRatio*range, false);
    setHistogramXMin(xMax() - windowRange, true);
    return true;
  }
  return false;
}

void Store::memoizeValidSeries(int series) {
  assert(series >= 0 && series < k_numberOfSeries);
  m_validSeries[series] = numberOfPairsOfSeries(series) > 0 && sumOfOccurrences(series) > 0;
}

bool Store::frequenciesAreInteger(int series) const {
  for (const double freq : m_data[series][1]) {
    if (std::fabs(freq - std::round(freq)) > DBL_EPSILON) {
      return false;
    }
  }
  return true;
}

/* Calculation */

double Store::sumOfOccurrences(int series) const {
  return sumOfColumn(series, 1);
}

double Store::maxValueForAllSeries(bool ignoreFrequency) const {
  assert(DoublePairStore::k_numberOfSeries > 0);
  double result = maxValue(0, ignoreFrequency);
  for (int i = 1; i < DoublePairStore::k_numberOfSeries; i++) {
    double maxCurrentSeries = maxValue(i, ignoreFrequency);
    if (result < maxCurrentSeries) {
      result = maxCurrentSeries;
    }
  }
  return result;
}

double Store::minValueForAllSeries(bool ignoreFrequency) const {
  assert(DoublePairStore::k_numberOfSeries > 0);
  double result = minValue(0, ignoreFrequency);
  for (int i = 1; i < DoublePairStore::k_numberOfSeries; i++) {
    double minCurrentSeries = minValue(i, ignoreFrequency);
    if (result > minCurrentSeries) {
      result = minCurrentSeries;
    }
  }
  return result;
}

double Store::maxValue(int series, bool ignoreFrequency) const {
  double max = -DBL_MAX;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    if (m_data[series][0][k] > max && (ignoreFrequency || m_data[series][1][k]) > 0) {
      max = m_data[series][0][k];
    }
  }
  return max;
}

double Store::minValue(int series, bool ignoreFrequency) const {
  double min = DBL_MAX;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    if (m_data[series][0][k] < min && (ignoreFrequency || m_data[series][1][k] > 0)) {
      min = m_data[series][0][k];
    }
  }
  return min;
}

double Store::range(int series) const {
  return maxValue(series)-minValue(series);
}

double Store::mean(int series) const {
  return sum(series)/sumOfOccurrences(series);
}

double Store::variance(int series) const {
  /* We use the Var(X) = E[(X-E[X])^2] definition instead of Var(X) = E[X^2] - E[X]^2
   * to ensure a positive result and to minimize rounding errors */
  double m = mean(series);
  return squaredOffsettedValueSum(series, m)/sumOfOccurrences(series);
}

double Store::standardDeviation(int series) const {
  return std::sqrt(variance(series));
}

double Store::sampleStandardDeviation(int series) const {
  double n = sumOfOccurrences(series);
  double s = std::sqrt(n/(n-1.0));
  return s*standardDeviation(series);
}

/* Below is the equivalence between quartiles and cumulated population, for the
 * international definition of quartiles (as medians of the lower and upper
 * half-lists). Let N be the total population, and k an integer.
 *
 *           Data repartition   Cumulated population
 *              Q1      Q3        Q1       Q3
 *
 * N = 4k    --- --- --- ---      k        3k                --- k elements
 * N = 4k+1  --- ---O––– ---      k        3k+1               O  1 element
 * N = 4k+2  ---O--- ---O---      k+1/2    3k+1+1/2
 * N = 4k+3  ---O---O---O---      k+1/2    3k+2+1/2
 *
 * Using floor(N/2)/2 as the cumulated population for Q1 and ceil(3N/2)/2 for
 * Q3 gives the right results.
 *
 * As this method is not well defined for rational frequencies, we escape to
 * the more general definition if non-integral frequencies are found.
 * */
double Store::firstQuartile(int series) const {
  if (GlobalPreferences::sharedGlobalPreferences()->methodForQuartiles() == CountryPreferences::MethodForQuartiles::CumulatedFrequency || !frequenciesAreInteger(series)) {
    return sortedElementAtCumulatedFrequency(series, 1.0/4.0);
  }
  assert(GlobalPreferences::sharedGlobalPreferences()->methodForQuartiles() == CountryPreferences::MethodForQuartiles::MedianOfSublist);
  return sortedElementAtCumulatedPopulation(series, std::floor(sumOfOccurrences(series) / 2.) / 2., true);
}

double Store::thirdQuartile(int series) const {
  if (GlobalPreferences::sharedGlobalPreferences()->methodForQuartiles() == CountryPreferences::MethodForQuartiles::CumulatedFrequency || !frequenciesAreInteger(series)) {
    return sortedElementAtCumulatedFrequency(series, 3.0/4.0);
  }
  assert(GlobalPreferences::sharedGlobalPreferences()->methodForQuartiles() == CountryPreferences::MethodForQuartiles::MedianOfSublist);
  return sortedElementAtCumulatedPopulation(series, std::ceil(3./2. * sumOfOccurrences(series)) / 2., true);
}

double Store::quartileRange(int series) const {
  return thirdQuartile(series)-firstQuartile(series);
}

double Store::median(int series) const {
  return sortedElementAtCumulatedFrequency(series, 1.0/2.0, true);
}

double Store::sum(int series) const {
  double result = 0;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    result += m_data[series][0][k]*m_data[series][1][k];
  }
  return result;
}

double Store::squaredValueSum(int series) const {
  return squaredOffsettedValueSum(series, 0.0);
}

double Store::squaredOffsettedValueSum(int series, double offset) const {
  double result = 0;
  const int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    double value = m_data[series][0][k] - offset;
    result += value*value*m_data[series][1][k];
  }
  return result;
}

bool Store::deleteValueAtIndex(int series, int i, int j) {
  deletePairOfSeriesAtIndex(series, j);
  return true;
}

/* Private methods */

double Store::defaultValue(int series, int i, int j) const {
  return (i == 0 && j > 1) ? 2 * m_data[series][i][j-1] - m_data[series][i][j-2] : 1.0;
}

double Store::sumOfValuesBetween(int series, double x1, double x2) const {
  double result = 0;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    if (m_data[series][0][k] < x2 && x1 <= m_data[series][0][k]) {
      result += m_data[series][1][k];
    }
  }
  return result;
}

double Store::sortedElementAtCumulatedFrequency(int series, double k, bool createMiddleElement) const {
  assert(k >= 0.0 && k <= 1.0);
  return sortedElementAtCumulatedPopulation(series, k * sumOfOccurrences(series), createMiddleElement);
}

double Store::sortedElementAtCumulatedPopulation(int series, double population, bool createMiddleElement) const {
  // TODO: use another algorithm (ex quickselect) to avoid quadratic complexity
  int numberOfPairs = numberOfPairsOfSeries(series);
  double bufferValues[k_maxNumberOfPairs];
  memcpy(bufferValues, m_data[series][0], numberOfPairs*sizeof(double));
  int sortedElementIndex = 0;
  double cumulatedNumberOfElements = 0.0;
  while (cumulatedNumberOfElements < population-DBL_EPSILON) {
    sortedElementIndex = minIndex(bufferValues, numberOfPairs);
    bufferValues[sortedElementIndex] = DBL_MAX;
    cumulatedNumberOfElements += m_data[series][1][sortedElementIndex];
  }

  if (createMiddleElement && std::fabs(cumulatedNumberOfElements - population) < DBL_EPSILON) {
    /* There is an element of cumulated frequency k, so the result is the mean
     * between this element and the next element (in terms of cumulated
     * frequency) that has a non-null frequency. */
    int nextElementIndex = minIndex(bufferValues, numberOfPairs);
    while (m_data[series][1][nextElementIndex] == 0 && bufferValues[nextElementIndex] != DBL_MAX) {
      bufferValues[nextElementIndex] = DBL_MAX;
      nextElementIndex = minIndex(bufferValues, numberOfPairs);
    }
    if (bufferValues[nextElementIndex] != DBL_MAX) {
      return (m_data[series][0][sortedElementIndex] + m_data[series][0][nextElementIndex]) / 2.0;
    }
  }

  return m_data[series][0][sortedElementIndex];
}

void Store::buildSortedIndex(int series, int * sortedIndex) const {
  // TODO : Factorize with Regression::Store::sortIndexByColumn
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int i = 0; i < numberOfPairs; i++) {
    sortedIndex[i] = i;
  }
  /* Following lines is an insertion-sort algorithm which has the advantage of
   * being in-place and efficient when already sorted. */
  int i = 1;
  while (i < numberOfPairs) {
    int xIndex = sortedIndex[i];
    double x = m_data[series][0][xIndex];
    int j = i - 1;
    while (j >= 0 && m_data[series][0][sortedIndex[j]] > x) {
      sortedIndex[j+1] = sortedIndex[j];
      j--;
    }
    sortedIndex[j+1] = xIndex;
    i++;
  }
}

int Store::totalCumulatedFrequencyValues(int series, int * sortedIndex) const {
  int distinctValues = 0;
  double x;
  for (size_t j = 0; j < numberOfPairsOfSeries(series); j++) {
    double nextX = get(series, 0, sortedIndex[j]);
    if (j == 0 || x != nextX) {
      distinctValues++;
      x = nextX;
    }
  }
  return distinctValues;
}

double Store::cumulatedFrequencyValueAtIndex(int series, int * sortedIndex, int i) const {
  int distinctValues = 0;
  double x;
  for (size_t j = 0; j < numberOfPairsOfSeries(series); j++) {
    double nextX = get(series, 0, sortedIndex[j]);
    if (j == 0 || x != nextX) {
      distinctValues++;
      x = nextX;
    }
    if (i == distinctValues - 1) {
      // Found the i-th distinct value
      return x;
    }
  }
  assert(false);
  return NAN;
}

double Store::cumulatedFrequencyResultAtIndex(int series, int * sortedIndex, int i) const {
  double cumulatedOccurrences = 0.0, otherOccurrences = 0.0;
  double value = cumulatedFrequencyValueAtIndex(series, sortedIndex, i);
  // Recompute sumOfOccurrences() here to save some computation.
  for (size_t j = 0; j < numberOfPairsOfSeries(series); j++) {
    double x = get(series, 0, sortedIndex[j]);
    (x <= value ? cumulatedOccurrences : otherOccurrences) += get(series, 1, sortedIndex[j]);
  }
  assert(cumulatedOccurrences + otherOccurrences == sumOfOccurrences(series));
  return 100.0*cumulatedOccurrences/(cumulatedOccurrences + otherOccurrences);
}

int Store::totalNormalProbabilityValues(int series) const {
  if (!frequenciesAreInteger(series)) {
    return 0;
  }
  return static_cast<int>(std::round(sumOfOccurrences(series)));
}

double Store::normalProbabilityValueAtIndex(int series, int * sortedIndex, int i) const {
  /* We could get rid of sortedIndex here by returning
   * sortedElementAtCumulatedPopulation(series, i + 1). However, this would sort
   * the series at each call. */
  // TODO : Handle situations where frequencies overflow or aren't integers
  assert(frequenciesAreInteger(series));
  int population = 0;
  for (size_t j = 0; j < numberOfPairsOfSeries(series); j++) {
    double frequency = std::round(get(series, 1, sortedIndex[j]));
    assert(frequency < static_cast<double>(INT_MAX));
    int frequencyInt = static_cast<int>(frequency);
    assert(frequencyInt < INT_MAX - population);
    population += frequencyInt;
    if (population > i) {
      assert(get(series, 0, sortedIndex[j]) == sortedElementAtCumulatedPopulation(series, i + 1));
      return get(series, 0, sortedIndex[j]);
    }
  }
  assert(false);
  return NAN;
}

double Store::normalProbabilityResultAtIndex(int series, int i) const {
  int total = totalNormalProbabilityValues(series);
  assert(total > 0);
  // invnorm((i-0.5)/total,0,1)
  double plottingPosition = (static_cast<double>(i)+0.5f)/static_cast<double>(total);
  return Poincare::NormalDistribution::CumulativeDistributiveInverseForProbability<double>(plottingPosition, 0.0, 1.0);
}

int Store::minIndex(double * bufferValues, int bufferLength) const {
  int index = 0;
  for (int i = 1; i < bufferLength; i++) {
    if (bufferValues[index] > bufferValues[i]) {
      index = i;
    }
  }
  return index;
}

}
