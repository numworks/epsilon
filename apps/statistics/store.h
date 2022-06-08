#ifndef STATISTICS_STORE_H
#define STATISTICS_STORE_H

#include <apps/i18n.h>
#include <apps/shared/double_pair_store.h>
#include <poincare/statistics_dataset.h>
#include <stddef.h>
#include <string.h>
#include "user_preferences.h"

namespace Statistics {

class Store : public Shared::DoublePairStore {
friend class BoxRange;
public:
  constexpr static const char * const * k_columnNames = DoublePairStore::k_statisticsColumNames;

  Store(Shared::GlobalContext * context, UserPreferences * userPreferences);

  void invalidateSortedIndexes();
  bool graphViewHasBeenInvalidated() const { return m_graphViewInvalidated; }
  void graphViewHasBeenSelected() { m_graphViewInvalidated = false; }
  int validSeriesIndex(int series, ValidSeries = &DefaultValidSeries) const;
  bool displayCumulatedFrequenciesForSeries(int series) const { return m_userPreferences->displayCumulatedFrequencies(series); }
  void setDisplayCumulatedFrequenciesForSeries(int series, bool state) { m_userPreferences->setDisplayCumulatedFrequencies(series, state); }
  int seriesAtColumn(int column) const override { return computeRelativeColumnAndSeries(&column); }
  int relativeColumnIndex(int columnIndex) const override;

  // DoublePairStore
  char columnNamePrefixAtIndex(int column) const override {
    assert(column >= 0 && column < DoublePairStore::k_numberOfColumnsPerSeries);
    assert(strlen(k_columnNames[column]) == 1);
    return k_columnNames[column][0];
  }

  // Histogram bars
  double barWidth() const { return m_userPreferences->barWidth(); }
  void setBarWidth(double barWidth);
  double firstDrawnBarAbscissa() const { return m_userPreferences->firstDrawnBarAbscissa(); }
  void setFirstDrawnBarAbscissa(double firstDrawnBarAbscissa) { m_userPreferences->setFirstDrawnBarAbscissa(firstDrawnBarAbscissa);}
  double heightOfBarAtIndex(int series, int index) const;
  double maxHeightOfBar(int series) const;
  double heightOfBarAtValue(int series, double value) const;
  double startOfBarAtIndex(int series, int index) const;
  double endOfBarAtIndex(int series, int index) const;
  int numberOfBars(int series) const;
  // Box plot
  bool displayOutliers() const { return m_userPreferences->displayOutliers(); }
  void setDisplayOutliers(bool displayOutliers) { m_userPreferences->setDisplayOutliers(displayOutliers); }
  I18n::Message boxPlotCalculationMessageAtIndex(int series, int index) const;
  double boxPlotCalculationAtIndex(int series, int index) const;
  bool boxPlotCalculationIsOutlier(int series, int index) const;
  int numberOfBoxPlotCalculations(int series) const;
  bool columnIsIntegersOnly(int series, int column) const;

  // Calculation
  double sumOfOccurrences(int series) const;
  // If handleNullFrequencies, values with a null frequency are accounted for
  double maxValueForAllSeries(bool handleNullFrequencies = false, ValidSeries = &DefaultValidSeries) const;
  double minValueForAllSeries(bool handleNullFrequencies = false, ValidSeries = &DefaultValidSeries) const;
  double maxValue(int series, bool handleNullFrequencies) const;
  double minValue(int series, bool handleNullFrequencies) const;
  // Overloading minValue and maxValue so they can be casted as CalculPointer
  double maxValue(int series) const { return maxValue(series, false); }
  double minValue(int series) const { return minValue(series, false); }
  double range(int series) const;
  double mean(int series) const;
  double variance(int series) const;
  double standardDeviation(int series) const;
  double sampleStandardDeviation(int series) const;
  double sampleVariance(int series) const;
  double firstQuartile(int series) const;
  double thirdQuartile(int series) const;
  double quartileRange(int series) const;
  double median(int series) const;
  double lowerWhisker(int series) const;
  double upperWhisker(int series) const;
  double lowerFence(int series) const;
  double upperFence(int series) const;
  int numberOfLowerOutliers(int series) const;
  int numberOfUpperOutliers(int series) const;
  double lowerOutlierAtIndex(int series, int index) const;
  double upperOutlierAtIndex(int series, int index) const;
  double sum(int series) const;
  double squaredValueSum(int series) const;
  int numberOfModes(int series) const;
  bool shouldDisplayModes(int series) const;
  int totalNumberOfModes() const;
  double modeAtIndex(int series, int index) const;
  double modeFrequency(int series) const;
  double sumOfValuesBetween(int series, double x1, double x2, bool strictUpperBound = true) const;

  /* Cumulated frequencies graphs:
   * Distinct values are aggregated and their frequency summed. */
  // Return number of distinct values
  int totalCumulatedFrequencyValues(int series) const;
  // Return the i-th distinct sorted value
  double cumulatedFrequencyValueAtIndex(int series, int i) const;
  // Return the cumulated frequency of the i-th distinct sorted value
  double cumulatedFrequencyResultAtIndex(int series, int i) const;

  /* Normal probability graphs:
   * Values are scattered into elements of frequency 1. */
  /* Return the sumOfOccurrences, return 0 if it exceeds k_maxNumberOfPairs or
   * if any frequency is not an integer */
  int totalNormalProbabilityValues(int series) const;
  // Return the sorted element at cumulated population i+1
  double normalProbabilityValueAtIndex(int series, int i) const;
  // Return the z-score of the i-th sorted element
  double normalProbabilityResultAtIndex(int series, int i) const;

  // DoublePairStore
  void updateSeriesValidity(int series) override;
  bool deleteValueAtIndex(int series, int i, int j, bool delayUpdate = false) override;

  typedef double (Store::*CalculPointer)(int) const;
  static bool ValidSeriesAndValidTotalNormalProbabilities(const DoublePairStore * store, int series) {
    return store->seriesIsValid(series) && static_cast<const Store *>(store)->totalNormalProbabilityValues(series) > 0;
  }
  bool updateSeries(int series, bool delayUpdate = false) override;
private:
  constexpr static int k_numberOfQuantiles = 5;
  constexpr static I18n::Message k_quantilesName[k_numberOfQuantiles] = {
    I18n::Message::StatisticsBoxLowerWhisker,
    I18n::Message::FirstQuartile,
    I18n::Message::Median,
    I18n::Message::ThirdQuartile,
    I18n::Message::StatisticsBoxUpperWhisker
  };
  constexpr static CalculPointer k_quantileCalculation[k_numberOfQuantiles] = {
    &Store::lowerWhisker,
    &Store::firstQuartile,
    &Store::median,
    &Store::thirdQuartile,
    &Store::upperWhisker
  };

  int computeRelativeColumnAndSeries(int * i) const;

  // DoublePairStore
  double defaultValue(int series, int i, int j) const override;
  /* Find the i-th distinct value (if i is -1, browse the entire series) from
   * start to end (ordered by value).
   * Retrieve the i-th value and the number distinct values encountered.
   * If not handleNullFrequencies, ignore values with null frequency. */
  void countDistinctValues(int series, int start, int end, int i, bool handleNullFrequencies, double * value, int * distinctValues) const;
  /* Find the i-th mode (ordered by value). Also retrieve the total number of
   * modes and the mode frequency. */
  double computeModes(int series, int i, double * modeFreq, int * modesTotal) const;
  double sortedElementAtCumulatedFrequency(int series, double k, bool createMiddleElement = false) const;
  double sortedElementAtCumulatedPopulation(int series, double population, bool createMiddleElement = false) const;
  uint8_t lowerWhiskerSortedIndex(int series) const;
  uint8_t upperWhiskerSortedIndex(int series) const;
  // Return the value index from its sorted index (a 0 sorted index is the min)
  uint8_t valueIndexAtSortedIndex(int series, int i) const;
  bool frequenciesAreValid(int series) const;

  UserPreferences * m_userPreferences;
  // Sorted value indexes are memoized to save computation
  static_assert(k_maxNumberOfPairs <= UINT8_MAX, "k_maxNumberOfPairs is too large.");
  /* The dataset memoizes the sorted indexes */
  Poincare::StatisticsDataset<double> m_datasets[k_numberOfSeries];
  /* Memoizing the max number of modes because the CalculationControllers needs
   * it in numberOfRows(), which is used a lot. */
  mutable int m_memoizedMaxNumberOfModes;
  bool m_graphViewInvalidated;
};

}

#endif
