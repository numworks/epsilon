#include "store.h"
#include <apps/global_preferences.h>
#include <poincare/normal_distribution.h>
#include <assert.h>
#include <algorithm>
#include <float.h>
#include <cmath>
#include <string.h>
#include <limits.h>

using namespace Shared;

namespace Statistics {

static_assert(Store::k_numberOfSeries == 3, "The constructor of Statistics::Store should be changed");

constexpr I18n::Message Store::k_quantilesName[Store::k_numberOfQuantiles];
constexpr Store::CalculPointer Store::k_quantileCalculation[Store::k_numberOfQuantiles];

Store::Store(GlobalContext * context, UserPreferences * userPreferences) :
  DoublePairStore(context),
  m_userPreferences(userPreferences),
  m_memoizedMaxNumberOfModes(-1),
  m_graphViewInvalidated(true)
{
  initListsFromStorage();
  for (int s = 0; s < k_numberOfSeries; s++) {
    m_datasets[s] = Poincare::StatisticsDataset<double>(&m_dataLists[s][0], &m_dataLists[s][1]);
  }
}

void Store::invalidateSortedIndexes() {
  for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
    m_datasets[i].setHasBeenModified();
  }
}

int Store::validSeriesIndex(int series, ValidSeries validSeries) const {
  assert(validSeries(this, series));
  int index = 0;
  for (int i = 0; i < series; i++) {
    index += validSeries(this, i);
  }
  return index;
}

/* Data */

int Store::relativeColumnIndex(int i) const {
  computeRelativeColumnAndSeries(&i);
  return i;
}

/* Histogram bars */

void Store::setBarWidth(double barWidth) {
  assert(barWidth > 0.0);
  m_userPreferences->setBarWidth(barWidth);
}

double Store::heightOfBarAtIndex(int series, int index) const {
  return sumOfValuesBetween(series, startOfBarAtIndex(series, index), endOfBarAtIndex(series, index));
}

double Store::maxHeightOfBar(int series) const {
  assert(seriesIsValid(series));
  double maxHeight = -DBL_MAX;
  double endOfBar = startOfBarAtIndex(series, 0);
  const int myNumberOfBars = numberOfBars(series);
  for (int i = 0; i < myNumberOfBars; i++) {
    // Reuse previous endOfBarAtIndex result
    double startOfBar = endOfBar;
    endOfBar = endOfBarAtIndex(series, i);
    maxHeight = std::max(maxHeight, sumOfValuesBetween(series, startOfBar, endOfBar));
  }
  assert(maxHeight > 0.0);
  return maxHeight;
}

double Store::heightOfBarAtValue(int series, double value) const {
  double width = barWidth();
  int barNumber = std::floor((value - firstDrawnBarAbscissa())/width);
  double lowerBound = firstDrawnBarAbscissa() + static_cast<double>(barNumber) * width;
  double upperBound = firstDrawnBarAbscissa() + static_cast<double>(barNumber + 1) * width;
  return sumOfValuesBetween(series, lowerBound, upperBound);
}

double Store::startOfBarAtIndex(int series, int index) const {
  double minimalValue = minValue(series);
  /* Because of floating point approximation, firstBarAbscissa could be lesser
   * than the minimal value. As a result, we would compute a height of zero for
   * all bars. */
  double firstBarAbscissa = std::min(minimalValue, firstDrawnBarAbscissa() + barWidth() * std::floor((minimalValue - firstDrawnBarAbscissa())/ barWidth()));
  return firstBarAbscissa + index * barWidth();
}

double Store::endOfBarAtIndex(int series, int index) const {
  return startOfBarAtIndex(series, index+1);
}

int Store::numberOfBars(int series) const {
  double firstBarAbscissa = startOfBarAtIndex(series, 0);
  return static_cast<int>(std::ceil((maxValue(series) - firstBarAbscissa) / barWidth())+1);
}

I18n::Message Store::boxPlotCalculationMessageAtIndex(int series, int index) const {
  if (index == 0) {
    return I18n::Message::Minimum;
  }
  int nbOfLowerOutliers = numberOfLowerOutliers(series);
  int nbOfUpperOutliers = numberOfUpperOutliers(series);
  if (index == nbOfLowerOutliers + k_numberOfQuantiles + nbOfUpperOutliers - 1) {
    // Handle maximum here to override UpperWhisker and Outlier messages
    return I18n::Message::Maximum;
  }
  if (index >= nbOfLowerOutliers && index < nbOfLowerOutliers + k_numberOfQuantiles) {
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
  return index < nbOfLowerOutliers || index >= nbOfLowerOutliers + k_numberOfQuantiles;
}

int Store::numberOfBoxPlotCalculations(int series) const {
  // Outliers + Lower/Upper Whisker + First/Third Quartile + Median
  return numberOfLowerOutliers(series) + k_numberOfQuantiles + numberOfUpperOutliers(series);
}

void Store::updateSeriesValidity(int series) {
  assert(series >= 0 && series < k_numberOfSeries);
  bool oldValidity = m_validSeries[series];
  DoublePairStore::updateSeriesValidity(series);
  m_validSeries[series] = m_validSeries[series] && frequenciesAreValid(series);
  // Reset the graph view any time one of the series gets invalidated
  m_graphViewInvalidated = m_graphViewInvalidated || (oldValidity && !m_validSeries[series]);
  if (m_graphViewInvalidated && numberOfPairsOfSeries(series) == 0) {
    // Hide the cumulated frequencies if series is invalidated and empty
    m_userPreferences->setDisplayCumulatedFrequencies(series, false);
  }
}

bool Store::columnIsIntegersOnly(int series, int column) const {
  for (int i = 0; i < numberOfPairsOfSeries(series); i++) {
    double freq = get(series, column, i);
    if (std::fabs(freq - std::round(freq)) > DBL_EPSILON) {
      return false;
    }
  }
  return true;
}

/* Calculation */

double Store::sumOfOccurrences(int series) const {
  return m_datasets[series].totalWeight();
}

double Store::maxValueForAllSeries(bool handleNullFrequencies, ValidSeries validSeries) const {
  assert(DoublePairStore::k_numberOfSeries > 0);
  double result = -DBL_MAX;
  for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
    if (validSeries(this, i)) {
      double maxCurrentSeries = maxValue(i, handleNullFrequencies);
      if (result < maxCurrentSeries) {
        result = maxCurrentSeries;
      }
    }
  }
  return result;
}

double Store::minValueForAllSeries(bool handleNullFrequencies, ValidSeries validSeries) const {
  assert(DoublePairStore::k_numberOfSeries > 0);
  double result = DBL_MAX;
  for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
    if (validSeries(this, i)) {
      double minCurrentSeries = minValue(i, handleNullFrequencies);
      if (result > minCurrentSeries) {
        result = minCurrentSeries;
      }
    }
  }
  return result;
}

double Store::maxValue(int series, bool handleNullFrequencies) const {
  assert(seriesIsValid(series));
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = numberOfPairs - 1; k >= 0; k--) {
    // Unless handleNullFrequencies is true, look for the last non null value.
    int sortedIndex = valueIndexAtSortedIndex(series, k);
    if (handleNullFrequencies || get(series, 1, sortedIndex) > 0.0) {
      return get(series, 0, sortedIndex);
    }
  }
  return -DBL_MAX;
}

double Store::minValue(int series, bool handleNullFrequencies) const {
  assert(seriesIsValid(series));
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    // Unless handleNullFrequencies is true, look for the first non null value.
    int sortedIndex = valueIndexAtSortedIndex(series, k);
    if (handleNullFrequencies || get(series, 1, sortedIndex) > 0.0) {
      return get(series, 0, sortedIndex);
    }
  }
  return DBL_MAX;
}

double Store::range(int series) const {
  return maxValue(series)-minValue(series);
}

double Store::mean(int series) const {
  return m_datasets[series].mean();
}

double Store::variance(int series) const {
  return m_datasets[series].variance();
}

double Store::standardDeviation(int series) const {
  return m_datasets[series].standardDeviation();
}

double Store::sampleStandardDeviation(int series) const {
  return m_datasets[series].sampleStandardDeviation();
}

double Store::sampleVariance(int series) const {
  double sampleStddev = sampleStandardDeviation(series);
  return sampleStddev * sampleStddev;
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
  if (GlobalPreferences::sharedGlobalPreferences()->methodForQuartiles() == CountryPreferences::MethodForQuartiles::CumulatedFrequency || !columnIsIntegersOnly(series, 1)) {
    return sortedElementAtCumulatedFrequency(series, 1.0/4.0);
  }
  assert(GlobalPreferences::sharedGlobalPreferences()->methodForQuartiles() == CountryPreferences::MethodForQuartiles::MedianOfSublist);
  return sortedElementAtCumulatedPopulation(series, std::floor(sumOfOccurrences(series) / 2.) / 2., true);
}

double Store::thirdQuartile(int series) const {
  if (GlobalPreferences::sharedGlobalPreferences()->methodForQuartiles() == CountryPreferences::MethodForQuartiles::CumulatedFrequency || !columnIsIntegersOnly(series, 1)) {
    return sortedElementAtCumulatedFrequency(series, 3.0/4.0);
  }
  assert(GlobalPreferences::sharedGlobalPreferences()->methodForQuartiles() == CountryPreferences::MethodForQuartiles::MedianOfSublist);
  return sortedElementAtCumulatedPopulation(series, std::ceil(3./2. * sumOfOccurrences(series)) / 2., true);
}

double Store::quartileRange(int series) const {
  return thirdQuartile(series)-firstQuartile(series);
}

double Store::median(int series) const {
  return m_datasets[series].median();
}

double Store::lowerWhisker(int series) const {
  return get(series, 0, valueIndexAtSortedIndex(series, lowerWhiskerSortedIndex(series)));
}

double Store::upperWhisker(int series) const {
  return get(series, 0, valueIndexAtSortedIndex(series, upperWhiskerSortedIndex(series)));
}

double Store::lowerFence(int series) const {
  return firstQuartile(series) - 1.5 * quartileRange(series);
}

double Store::upperFence(int series) const {
  return thirdQuartile(series) + 1.5 * quartileRange(series);
}

int Store::numberOfLowerOutliers(int series) const {
  if (!displayOutliers()) {
    return 0;
  }
  double value;
  int distinctValues;
  countDistinctValues(series, 0, lowerWhiskerSortedIndex(series), -1, false, &value, &distinctValues);
  return distinctValues;
}

int Store::numberOfUpperOutliers(int series) const {
  if (!displayOutliers()) {
    return 0;
  }
  double value;
  int distinctValues;
  countDistinctValues(series, upperWhiskerSortedIndex(series) + 1, numberOfPairsOfSeries(series), -1, false, &value, &distinctValues);
  return distinctValues;
}

double Store::lowerOutlierAtIndex(int series, int index) const {
  assert(displayOutliers() && index < numberOfLowerOutliers(series));
  double value;
  int distinctValues;
  countDistinctValues(series, 0, lowerWhiskerSortedIndex(series), index, false, &value, &distinctValues);
  return value;
}

double Store::upperOutlierAtIndex(int series, int index) const {
  assert(displayOutliers() && index < numberOfUpperOutliers(series));
  double value;
  int distinctValues;
  countDistinctValues(series, upperWhiskerSortedIndex(series) + 1, numberOfPairsOfSeries(series), index, false, &value, &distinctValues);
  return value;
}

double Store::sum(int series) const {
  return m_datasets[series].weightedSum();
}

double Store::squaredValueSum(int series) const {
  return m_datasets[series].squaredSum();
}

int Store::numberOfModes(int series) const {
  double modeFreq;
  int modesTotal;
  computeModes(series, -1, &modeFreq, &modesTotal);
  return modesTotal;
}

bool Store::shouldDisplayModes(int series) const {
  return columnIsIntegersOnly(series, 1) && modeFrequency(series) != 1.f;
}

int Store::totalNumberOfModes() const {
  if (m_memoizedMaxNumberOfModes >= 0) {
    return m_memoizedMaxNumberOfModes;
  }
  int maxNumberOfModes = 0;
  for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
    if (seriesIsValid(i) && shouldDisplayModes(i)) {
      maxNumberOfModes = std::max(maxNumberOfModes, numberOfModes(i));
    }
  }
  m_memoizedMaxNumberOfModes = maxNumberOfModes;
  return maxNumberOfModes;
}

double Store::modeAtIndex(int series, int index) const {
  double modeFreq;
  int modesTotal;
  return computeModes(series, index, &modeFreq, &modesTotal);
}

double Store::modeFrequency(int series) const {
  double modeFreq;
  int modesTotal;
  computeModes(series, -1, &modeFreq, &modesTotal);
  return modeFreq;
}

double Store::computeModes(int series, int i, double * modeFreq, int * modesTotal) const {
  *modesTotal = 0;
  *modeFreq = 0.0;
  double ithValue = NAN;
  double currentValue = NAN;
  double currentValueFrequency = NAN;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int j = 0; j <= numberOfPairs; j++) {
    double value, valueFrequency;
    if (j < numberOfPairs) {
      int valueIndex = valueIndexAtSortedIndex(series, j);
      value = get(series, 0, valueIndex);
      valueFrequency = get(series, 1, valueIndex);
    } else {
      // Iterating one last time to process the last value
      value = valueFrequency = NAN;
    }
    // currentValue != value returns true if currentValue or value is NAN
    if (currentValue != value) {
      // A new value has been found
      if (currentValueFrequency > *modeFreq) {
        // A better mode has been found, reset solutions
        *modeFreq = currentValueFrequency;
        *modesTotal = 0;
        ithValue = NAN;
      }
      if (currentValueFrequency == *modeFreq) {
        // Another mode has been found
        if (*modesTotal == i) {
          ithValue = currentValue;
        }
        *modesTotal += 1;
      }
      currentValueFrequency = 0.0;
      currentValue = value;
    }
    currentValueFrequency += valueFrequency;
  }
  // A valid total, frequency and ithValue (if requested) have been calculated
  assert(*modesTotal > 0 && *modeFreq > 0.0 && std::isnan(ithValue) == (i == -1));
  return ithValue;
}

bool Store::deleteValueAtIndex(int series, int i, int j, bool delayUpdate) {
  deletePairOfSeriesAtIndex(series, j, delayUpdate);
  return true;
}

/* Private methods */

int Store::computeRelativeColumnAndSeries(int * i) const {
  int seriesIndex = 0;
  while (*i >= DoublePairStore::k_numberOfColumnsPerSeries + displayCumulatedFrequenciesForSeries(seriesIndex)) {
    *i -= DoublePairStore::k_numberOfColumnsPerSeries + displayCumulatedFrequenciesForSeries(seriesIndex);
    seriesIndex++;
    assert(seriesIndex < k_numberOfSeries);
  }
  return seriesIndex;
}

double Store::defaultValue(int series, int i, int j) const {
  return (i == 0 && j > 1) ? 2 * get(series, i, j-1) - get(series, i, j-2) : 1.0;
}

bool Store::updateSeries(int series, bool delayUpdate) {
  m_datasets[series].setHasBeenModified();
  m_memoizedMaxNumberOfModes = -1;
  return DoublePairStore::updateSeries(series, delayUpdate);
}

double Store::sumOfValuesBetween(int series, double x1, double x2, bool strictUpperBound) const {
  if (!seriesIsValid(series)) {
    return NAN;
  }
  double result = 0;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    int sortedIndex = valueIndexAtSortedIndex(series, k);
    double value = get(series, 0, sortedIndex);
    if (value > x2 || (strictUpperBound && value == x2)) {
      break;
    }
    if (value >= x1) {
      result += get(series, 1, sortedIndex);
    }
  }
  return result;
}

double Store::sortedElementAtCumulatedFrequency(int series, double k, bool createMiddleElement) const {
  return m_datasets[series].sortedElementAtCumulatedFrequency(k, createMiddleElement);
}

double Store::sortedElementAtCumulatedPopulation(int series, double population, bool createMiddleElement) const {
  return m_datasets[series].sortedElementAtCumulatedWeight(population, createMiddleElement);
}

uint8_t Store::lowerWhiskerSortedIndex(int series) const {
  double lowFence = lowerFence(series);
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    int valueIndex = valueIndexAtSortedIndex(series, k);
    if ((!displayOutliers() || get(series, 0, valueIndex) >= lowFence) && get(series, 1, valueIndex) > 0.0) {
      return k;
    }
  }
  assert(false);
  return numberOfPairs;
}

uint8_t Store::upperWhiskerSortedIndex(int series) const {
  double uppFence = upperFence(series);
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = numberOfPairs - 1; k >= 0; k--) {
    int valueIndex = valueIndexAtSortedIndex(series, k);
    if ((!displayOutliers() || get(series, 0, valueIndex) <= uppFence) && get(series, 1, valueIndex) > 0.0) {
      return k;
    }
  }
  assert(false);
  return numberOfPairs;
}

void Store::countDistinctValues(int series, int start, int end, int i, bool handleNullFrequencies, double * value, int * distinctValues) const {
  assert(start >= 0 && end <= numberOfPairsOfSeries(series) && start <= end);
  *distinctValues = 0;
  *value = NAN;
  for (int j = start; j < end; j++) {
    int valueIndex = valueIndexAtSortedIndex(series, j);
    if (handleNullFrequencies || get(series, 1, valueIndex) > 0.0) {
      double nextX = get(series, 0, valueIndexAtSortedIndex(series, j));
      // *value != nextX returns true if *value is NAN
      if (*value != nextX) {
        (*distinctValues)++;
        *value = nextX;
      }
      if (i == (*distinctValues) - 1) {
        // Found the i-th distinct value
        return;
      }
    }
  }
  assert(i == -1);
}

int Store::totalCumulatedFrequencyValues(int series) const {
  double value;
  int distinctValues;
  countDistinctValues(series, 0, numberOfPairsOfSeries(series), -1, true, &value, &distinctValues);
  return distinctValues;
}

double Store::cumulatedFrequencyValueAtIndex(int series, int i) const {
  double value;
  int distinctValues;
  countDistinctValues(series, 0, numberOfPairsOfSeries(series), i, true, &value, &distinctValues);
  return value;
}

double Store::cumulatedFrequencyResultAtIndex(int series, int i) const {
  double cumulatedOccurrences = 0.0, otherOccurrences = 0.0;
  double value = cumulatedFrequencyValueAtIndex(series, i);
  // Recompute sumOfOccurrences() here to save some computation.
  const int numberOfPairs = numberOfPairsOfSeries(series);
  for (int j = 0; j < numberOfPairs; j++) {
    double x = get(series, 0, valueIndexAtSortedIndex(series, j));
    (x <= value ? cumulatedOccurrences : otherOccurrences) += get(series, 1, valueIndexAtSortedIndex(series, j));
  }
  assert(std::abs(cumulatedOccurrences + otherOccurrences - sumOfOccurrences(series)) <= FLT_EPSILON);
  return 100.0 * cumulatedOccurrences / (cumulatedOccurrences + otherOccurrences);
}

int Store::totalNormalProbabilityValues(int series) const {
  if (!seriesIsValid(series) || !columnIsIntegersOnly(series, 1)) {
    return 0;
  }
  double result = sumOfOccurrences(series);
  assert(result == std::round(result));
  /* Limiting the result to k_maxNumberOfPairs to prevent limitless Normal
   * Probability plots and int overflows */
  static_assert(k_maxNumberOfPairs <= INT_MAX, "k_maxNumberOfPairs is too large.");
  if (result > k_maxNumberOfPairs) {
    return 0;
  }
  return static_cast<int>(result);
}

double Store::normalProbabilityValueAtIndex(int series, int i) const {
  assert(columnIsIntegersOnly(series, 1));
  return sortedElementAtCumulatedPopulation(series, i + 1, false);
}

double Store::normalProbabilityResultAtIndex(int series, int i) const {
  double total = static_cast<double>(totalNormalProbabilityValues(series));
  assert(i >= 0 && total > 0.0 && static_cast<double>(i) < total);
  // invnorm((i-0.5)/total,0,1)
  double plottingPosition = (static_cast<double>(i) + 0.5) / total;
  return Poincare::NormalDistribution::CumulativeDistributiveInverseForProbability<double>(plottingPosition, 0.0, 1.0);
}

uint8_t Store::valueIndexAtSortedIndex(int series, int i) const {
  return m_datasets[series].indexAtSortedIndex(i);
}

bool Store::frequenciesAreValid(int series) const {
  int numberOfPairs = numberOfPairsOfSeries(series);
  bool onlyZeros = true;
  for (int i = 0; i < numberOfPairs; i++) {
    double frequency = get(series, 1, i);
    if (std::isnan(frequency) || frequency < 0.0) {
      return false;
    }
    if (onlyZeros && frequency > 0.0) {
      onlyZeros = false;
    }
  }
  return !onlyZeros;
}

}
