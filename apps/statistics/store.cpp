#include "store.h"
#include <apps/global_preferences.h>
#include <assert.h>
#include <float.h>
#include <cmath>
#include <string.h>
#include <ion.h>

using namespace Shared;

namespace Statistics {

static_assert(Store::k_numberOfSeries == 3, "The constructor of Statistics::Store should be changed");

Store::Store() :
  MemoizedCurveViewRange(),
  DoublePairStore(),
  m_barWidth(1.0),
  m_firstDrawnBarAbscissa(0.0),
  m_seriesEmpty{true, true, true},
  m_numberOfNonEmptySeries(0)
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
    m_xRange.setMin(startSelectedBar - k_displayLeftMarginRatio*range);
    // Call MemoizedCurveViewRange::protectedSetXMax to update the grid unit
    MemoizedCurveViewRange::protectedSetXMax(xMin() + windowRange);
    return true;
  }
  float endSelectedBar = endOfBarAtIndex(series, index);
  if (endSelectedBar > xMax() - k_displayRightMarginRatio*range) {
    m_xRange.setMax(endSelectedBar + k_displayRightMarginRatio*range);
    MemoizedCurveViewRange::protectedSetXMin(xMax() - windowRange);
    return true;
  }
  return false;
}

bool Store::isEmpty() const {
  return numberOfNonEmptySeries() == 0;
}

bool Store::seriesIsEmpty(int i) const {
  return m_seriesEmpty[i];
}

bool Store::frequenciesAreInteger(int series) const {
  for (const double freq : m_data[series][1]) {
    if (std::fabs(freq - std::round(freq)) > DBL_EPSILON) {
      return false;
    }
  }
  return true;
}

int Store::numberOfNonEmptySeries() const {
  return m_numberOfNonEmptySeries;
}

/* Calculation */

double Store::sumOfOccurrences(int series) const {
  return sumOfColumn(series, 1);
}

double Store::maxValueForAllSeries() const {
  assert(DoublePairStore::k_numberOfSeries > 0);
  double result = maxValue(0);
  for (int i = 1; i < DoublePairStore::k_numberOfSeries; i++) {
    double maxCurrentSeries = maxValue(i);
    if (result < maxCurrentSeries) {
      result = maxCurrentSeries;
    }
  }
  return result;
}

double Store::minValueForAllSeries() const {
  assert(DoublePairStore::k_numberOfSeries > 0);
  double result = minValue(0);
  for (int i = 1; i < DoublePairStore::k_numberOfSeries; i++) {
    double minCurrentSeries = minValue(i);
    if (result > minCurrentSeries) {
      result = minCurrentSeries;
    }
  }
  return result;
}

double Store::maxValue(int series) const {
  double max = -DBL_MAX;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    if (m_data[series][0][k] > max && m_data[series][1][k] > 0) {
      max = m_data[series][0][k];
    }
  }
  return max;
}

double Store::minValue(int series) const {
  double min = DBL_MAX;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    if (m_data[series][0][k] < min && m_data[series][1][k] > 0) {
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

double Store::geometricMean(int series) const {
  double geometricMean = 1;
  int numberOfCoefficients = 0;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    if (m_data[series][0][k] <= 0) {
      return NAN;
    }
    geometricMean *= std::pow(m_data[series][0][k], m_data[series][1][k]);
    numberOfCoefficients += m_data[series][1][k];
  }
  return std::pow(geometricMean, 1.0/numberOfCoefficients);
}

double Store::harmonicMean(int series) const {
  double harmonicMean = 0;
  int numberOfCoefficients = 0;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    if (m_data[series][0][k] <= 0) {
      return NAN;
    }
    harmonicMean += m_data[series][1][k]/m_data[series][0][k];
    numberOfCoefficients += m_data[series][1][k];
  }
  return numberOfCoefficients/harmonicMean;
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

double Store::mode(int series) const {
  double modeValue = NAN;
  double numberOfRepeats = 0;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    if (m_data[series][1][k] > numberOfRepeats) {
        modeValue = m_data[series][0][k];
        numberOfRepeats = m_data[series][1][k];
    }
    else if (m_data[series][1][k] == numberOfRepeats) {
      modeValue = NAN;
    }
  }
  return modeValue;
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

void Store::set(double f, int series, int i, int j) {
  DoublePairStore::set(f, series, i, j);
  m_seriesEmpty[series] = sumOfOccurrences(series) == 0;
  updateNonEmptySeriesCount();
}

void Store::deletePairOfSeriesAtIndex(int series, int j) {
  DoublePairStore::deletePairOfSeriesAtIndex(series, j);
  m_seriesEmpty[series] = sumOfOccurrences(series) == 0;
  updateNonEmptySeriesCount();
}

void Store::deleteAllPairsOfSeries(int series) {
  DoublePairStore::deleteAllPairsOfSeries(series);
  m_seriesEmpty[series] = true;
  updateNonEmptySeriesCount();
}

void Store::updateNonEmptySeriesCount() {
  int nonEmptySeriesCount = 0;
  for (int i = 0; i< k_numberOfSeries; i++) {
    if (!m_seriesEmpty[i]) {
      nonEmptySeriesCount++;
    }
  }
  m_numberOfNonEmptySeries = nonEmptySeriesCount;
}

/* Private methods */

double Store::defaultValue(int series, int i, int j) const {
  return i == 0 ? DoublePairStore::defaultValue(series, i, j) : 1.0;
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
