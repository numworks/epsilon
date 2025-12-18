#include "store.h"

#include <apps/global_preferences.h>
#include <assert.h>
#include <float.h>
#include <limits.h>
#include <poincare/statistics/distribution.h>
#include <string.h>

#include <algorithm>
#include <cmath>

using namespace Shared;

namespace Statistics {

constexpr I18n::Message Store::k_quantilesName[Store::k_numberOfQuantiles];
constexpr Store::CalculPointer
    Store::k_quantileCalculation[Store::k_numberOfQuantiles];

Store::Store(UserPreferences* userPreferences)
    : StatisticsStore(userPreferences),
      m_memoizedMaxNumberOfModes(-1),
      m_graphViewInvalidated(true) {
  initDatasets();
}

/* Data */

int Store::relativeColumn(int i) const {
  computeRelativeColumnAndSeries(&i);
  return i;
}

/* Histogram bars */

void Store::setBarWidth(float barWidth) {
  assert(barWidth > 0.f);
  userPreferences()->setBarWidth(barWidth);
}

float Store::heightOfBarAtIndex(int series, int index) const {
  return m_datasets[series].heightOfBarAtIndex(index, barWidth(),
                                               firstDrawnBarAbscissa());
}

float Store::maxHeightOfBar(int series) const {
  assert(seriesIsActive(series));
  float maxHeight = -FLT_MAX;
  float endOfBar = startOfBarAtIndex(series, 0);
  const int myNumberOfBars = numberOfBars(series);
  for (int i = 0; i < myNumberOfBars; i++) {
    // Reuse previous endOfBarAtIndex result
    float startOfBar = endOfBar;
    endOfBar = endOfBarAtIndex(series, i);
    maxHeight = std::max(maxHeight, static_cast<float>(sumOfValuesBetween(
                                        series, startOfBar, endOfBar)));
  }
  assert(maxHeight > 0.f);
  return maxHeight;
}

float Store::heightOfBarAtValue(int series, double value) const {
  float width = barWidth();
  int barNumber =
      std::floor((static_cast<float>(value) - firstDrawnBarAbscissa()) / width);
  float lowerBound =
      firstDrawnBarAbscissa() + static_cast<float>(barNumber) * width;
  float upperBound =
      firstDrawnBarAbscissa() + static_cast<float>(barNumber + 1) * width;
  return static_cast<float>(sumOfValuesBetween(series, lowerBound, upperBound));
}

float Store::startOfBarAtIndex(int series, int index) const {
  return m_datasets[series].startOfBarAtIndex(index, barWidth(),
                                              firstDrawnBarAbscissa());
}

float Store::endOfBarAtIndex(int series, int index) const {
  return m_datasets[series].endOfBarAtIndex(index, barWidth(),
                                            firstDrawnBarAbscissa());
}

int Store::numberOfBars(int series) const {
  double firstBarAbscissa = startOfBarAtIndex(series, 0);
  double maxVal = maxValue(series);
  int nBars = static_cast<int>(
      std::floor((maxVal - firstBarAbscissa) / barWidth()) + 1);
  if (OMG::Float::RelativelyEqual<double>(maxVal,
                                          firstBarAbscissa + nBars * barWidth(),
                                          OMG::Float::EpsilonLax<double>())) {
    /* If the maxValue is on the upper bound of the last bar, we need to add
     * one bar to be consistent with sumOfValuesBetween. */
    nBars++;
  }
  return nBars;
}

I18n::Message Store::boxPlotCalculationMessageAtIndex(int series,
                                                      int index) const {
  if (index == 0) {
    return I18n::Message::Minimum;
  }
  int nbOfLowerOutliers = numberOfLowerOutliers(series);
  int nbOfUpperOutliers = numberOfUpperOutliers(series);
  if (index ==
      nbOfLowerOutliers + k_numberOfQuantiles + nbOfUpperOutliers - 1) {
    // Handle maximum here to override UpperWhisker and Outlier messages
    return I18n::Message::Maximum;
  }
  if (index >= nbOfLowerOutliers &&
      index < nbOfLowerOutliers + k_numberOfQuantiles) {
    return k_quantilesName[index - nbOfLowerOutliers];
  }
  assert(index < nbOfLowerOutliers + k_numberOfQuantiles + nbOfUpperOutliers);
  return I18n::Message::StatisticsBoxOutlier;
}

double Store::boxPlotCalculationAtIndex(int series, int index) const {
  assert(index >= 0);
  if (index == 0) {
    return minValue(series);
  }
  int nbOfLowerOutliers = numberOfLowerOutliers(series);
  if (index < nbOfLowerOutliers) {
    return lowerOutlierAtIndex(series, index);
  }
  index -= nbOfLowerOutliers;
  if (index < k_numberOfQuantiles) {
    return (this->*k_quantileCalculation[index])(series);
  }
  index -= k_numberOfQuantiles;
  assert(index >= 0 && index < numberOfUpperOutliers(series));
  return upperOutlierAtIndex(series, index);
}

bool Store::boxPlotCalculationIsOutlier(int series, int index) const {
  int nbOfLowerOutliers = numberOfLowerOutliers(series);
  return index < nbOfLowerOutliers ||
         index >= nbOfLowerOutliers + k_numberOfQuantiles;
}

int Store::numberOfBoxPlotCalculations(int series) const {
  // Outliers + Lower/Upper Whisker + First/Third Quartile + Median
  return numberOfLowerOutliers(series) + k_numberOfQuantiles +
         numberOfUpperOutliers(series);
}

void Store::updateSeriesValidity(int series) {
  assert(series >= 0 && series < k_numberOfSeries);
  bool oldValidity = seriesIsValid(series);
  DoublePairStore::updateSeriesValidity(series);
  userPreferences()->setSeriesValid(
      series, seriesIsValid(series) && frequenciesAreValid(series));
  if (oldValidity && !seriesIsValid(series)) {
    // Reset the graph view any time one of the series gets invalidated
    m_graphViewInvalidated = true;
    if (numberOfPairsOfSeries(series) == 0) {
      // Hide the cumulated frequencies if series is invalidated and empty
      userPreferences()->setDisplayCumulatedFrequencies(series, false);
    }
  }
}

bool Store::columnIsIntegersOnly(int series, int column) const {
  for (int i = 0; i < numberOfPairsOfSeries(series); i++) {
    double freq = get(series, column, i);
    if (freq != std::round(freq)) {
      return false;
    }
  }
  return true;
}

/* Calculation */

double Store::maxValueForAllSeries(bool handleNullFrequencies,
                                   ActiveSeriesTest activeSeriesTest) const {
  assert(DoublePairStore::k_numberOfSeries > 0);
  double result = -DBL_MAX;
  for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
    if (activeSeriesTest(this, i)) {
      double maxCurrentSeries = maxValue(i, handleNullFrequencies);
      if (result < maxCurrentSeries) {
        result = maxCurrentSeries;
      }
    }
  }
  return result;
}

double Store::minValueForAllSeries(bool handleNullFrequencies,
                                   ActiveSeriesTest activeSeriesTest) const {
  assert(DoublePairStore::k_numberOfSeries > 0);
  double result = DBL_MAX;
  for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
    if (activeSeriesTest(this, i)) {
      double minCurrentSeries = minValue(i, handleNullFrequencies);
      if (result > minCurrentSeries) {
        result = minCurrentSeries;
      }
    }
  }
  return result;
}

double Store::maxValue(int series, bool handleNullFrequencies) const {
  assert(seriesIsActive(series));
  return handleNullFrequencies
             ? m_datasets[series].maxIncludingNullFrequencies()
             : m_datasets[series].max();
}

double Store::minValue(int series, bool handleNullFrequencies) const {
  assert(seriesIsActive(series));
  return handleNullFrequencies
             ? m_datasets[series].minIncludingNullFrequencies()
             : m_datasets[series].min();
}

double Store::range(int series) const {
  return maxValue(series) - minValue(series);
}

double Store::variance(int series) const {
  return m_datasets[series].variance();
}

double Store::sampleVariance(int series) const {
  return m_datasets[series].sampleVariance();
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
  return m_datasets[series].firstQuartile();
}

double Store::thirdQuartile(int series) const {
  return m_datasets[series].thirdQuartile();
}

double Store::quartileRange(int series) const {
  return m_datasets[series].quartileRange();
}

double Store::median(int series) const { return m_datasets[series].median(); }

double Store::lowerWhisker(int series) const {
  return m_datasets[series].lowerWhisker(displayOutliers());
}

double Store::upperWhisker(int series) const {
  return m_datasets[series].upperWhisker(displayOutliers());
}

double Store::lowerFence(int series) const {
  return m_datasets[series].lowerFence();
}

double Store::upperFence(int series) const {
  return m_datasets[series].upperFence();
}

int Store::numberOfLowerOutliers(int series) const {
  if (!displayOutliers()) {
    return 0;
  }
  return m_datasets[series].numberOfLowerOutliers();
}

int Store::numberOfUpperOutliers(int series) const {
  if (!displayOutliers()) {
    return 0;
  }
  return m_datasets[series].numberOfUpperOutliers();
}

double Store::lowerOutlierAtIndex(int series, int index) const {
  assert(displayOutliers() && index < numberOfLowerOutliers(series));
  return m_datasets[series].lowerOutlierAtIndex(index);
}

double Store::upperOutlierAtIndex(int series, int index) const {
  assert(displayOutliers() && index < numberOfUpperOutliers(series));
  return m_datasets[series].upperOutlierAtIndex(index);
}

double Store::sum(int series) const { return m_datasets[series].weightedSum(); }

double Store::squaredValueSum(int series) const {
  return m_datasets[series].squaredSum();
}

int Store::numberOfModes(int series) const {
  return m_datasets[series].modeData().numberOfModes;
}

bool Store::shouldDisplayModes(int series) const {
  /* We want to avoid to display hundreds of modes when the series is
   * automatically filled with ones */
  return modeFrequency(series) != 1.f;
}

int Store::totalNumberOfModes() const {
  if (m_memoizedMaxNumberOfModes >= 0) {
    return m_memoizedMaxNumberOfModes;
  }
  int maxNumberOfModes = 0;
  for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
    if (seriesIsActive(i) && shouldDisplayModes(i)) {
      maxNumberOfModes = std::max(maxNumberOfModes, numberOfModes(i));
    }
  }
  m_memoizedMaxNumberOfModes = maxNumberOfModes;
  return maxNumberOfModes;
}

double Store::modeAtIndex(int series, int index) const {
  return m_datasets[series].modeValueAtIndex(index);
}

double Store::modeFrequency(int series) const {
  return m_datasets[series].modeData().modeWeight;
}

/* Private methods */

int Store::computeRelativeColumnAndSeries(int* i) const {
  int seriesIndex = 0;
  while (*i >= DoublePairStore::k_numberOfColumnsPerSeries +
                   displayCumulatedFrequenciesForSeries(seriesIndex)) {
    *i -= DoublePairStore::k_numberOfColumnsPerSeries +
          displayCumulatedFrequenciesForSeries(seriesIndex);
    seriesIndex++;
    assert(seriesIndex < k_numberOfSeries);
  }
  return seriesIndex;
}

bool Store::updateSeries(int series, bool delayUpdate) {
  m_memoizedMaxNumberOfModes = -1;
  return StatisticsStore::updateSeries(series, delayUpdate);
}

double Store::sumOfValuesBetween(int series, double x1, double x2,
                                 bool strictUpperBound) const {
  if (!seriesIsValid(series)) {
    return NAN;
  }
  return m_datasets[series].sumOfValuesBetween(x1, x2, strictUpperBound);
}

double Store::sortedElementAtCumulatedFrequency(
    int series, double k, bool createMiddleElement) const {
  return m_datasets[series].sortedElementAtCumulatedFrequency(
      k, createMiddleElement);
}

double Store::sortedElementAtCumulatedPopulation(
    int series, double population, bool createMiddleElement) const {
  return m_datasets[series].sortedElementAtCumulatedWeight(population,
                                                           createMiddleElement);
}

uint8_t Store::lowerWhiskerSortedIndex(int series) const {
  return m_datasets[series].lowerWhiskerSortedIndex(displayOutliers());
}

uint8_t Store::upperWhiskerSortedIndex(int series) const {
  return m_datasets[series].upperWhiskerSortedIndex(displayOutliers());
}

void Store::countDistinctValues(int series, int start, int end, int i,
                                bool handleNullFrequencies, double* value,
                                int* distinctValues) const {
  return m_datasets[series].countDistinctValues(
      start, end, i, handleNullFrequencies, value, distinctValues);
}

int Store::totalCumulatedFrequencyValues(int series) const {
  return m_datasets[series].totalCumulatedFrequencyValues();
}

double Store::cumulatedFrequencyValueAtIndex(int series, int i) const {
  return m_datasets[series].cumulatedFrequencyValueAtIndex(i);
}

double Store::cumulatedFrequencyResultAtIndex(int series, int i) const {
  return 100.0 * m_datasets[series].cumulatedFrequencyResultAtIndex(i);
}

int Store::totalNormalProbabilityValues(int series) const {
  if (!seriesIsActive(series)) {
    return 0;
  }
  return m_datasets[series].totalNormalProbabilityValues();
}

double Store::normalProbabilityValueAtIndex(int series, int i) const {
  return m_datasets[series].normalProbabilityValueAtIndex(i);
}

double Store::normalProbabilityResultAtIndex(int series, int i) const {
  return m_datasets[series].normalProbabilityResultAtIndex(i);
}

double Store::normalProbabilityZScoreLineAtAbscissa(int series,
                                                    double x) const {
  return m_datasets[series].normalProbabilityZScoreLineAtAbscissa(x);
}

uint8_t Store::valueIndexAtSortedIndex(int series, int i) const {
  return m_datasets[series].indexAtSortedIndex(i);
}

bool Store::frequenciesAreValid(int series) const {
  assert(seriesIsValid(series));
  // Take advantage of total weight memoization
  return sumOfOccurrences(series) > 0.0;
}

}  // namespace Statistics
