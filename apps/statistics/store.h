#ifndef STATISTICS_STORE_H
#define STATISTICS_STORE_H

#include <apps/shared/memoized_curve_view_range.h>
#include <apps/shared/double_pair_store.h>
#include <stddef.h>

namespace Statistics {

class Store : public Shared::MemoizedCurveViewRange, public Shared::DoublePairStore {
friend class BoxRange;
public:
  Store();
  uint32_t barChecksum() const;
  // Histogram bars
  double barWidth() const { return m_barWidth; }
  void setBarWidth(double barWidth);
  double firstDrawnBarAbscissa() const { return m_firstDrawnBarAbscissa; }
  void setFirstDrawnBarAbscissa(double firstDrawnBarAbscissa) { m_firstDrawnBarAbscissa = firstDrawnBarAbscissa;}
  double heightOfBarAtIndex(int series, int index) const;
  double heightOfBarAtValue(int series, double value) const;
  double startOfBarAtIndex(int series, int index) const;
  double endOfBarAtIndex(int series, int index) const;
  double numberOfBars(int series) const;
  void setHistogramXMin(float f, bool updateGridUnit) { protectedSetXMin(f, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, updateGridUnit); }
  void setHistogramXMax(float f, bool updateGridUnit) { protectedSetXMax(f, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, updateGridUnit); }
  // return true if the window has scrolled
  bool scrollToSelectedBarIndex(int series, int index);
  bool frequenciesAreInteger(int series) const;

  // Calculation
  double sumOfOccurrences(int series) const;
  // If ignoreFrequency is true, values with a null frequency are accounted for
  double maxValueForAllSeries(bool ignoreFrequency = false) const;
  double minValueForAllSeries(bool ignoreFrequency = false) const;
  double maxValue(int series, bool ignoreFrequency) const;
  double minValue(int series, bool ignoreFrequency) const;
  // Overloading minValue and maxValue so they can be casted as CalculPointer
  double maxValue(int series) const { return maxValue(series, false); }
  double minValue(int series) const { return minValue(series, false); }
  double range(int series) const;
  double mean(int series) const;
  double variance(int series) const;
  double standardDeviation(int series) const;
  double sampleStandardDeviation(int series) const;
  double firstQuartile(int series) const;
  double thirdQuartile(int series) const;
  double quartileRange(int series) const;
  double median(int series) const;
  double sum(int series) const;
  double squaredValueSum(int series) const;
  double squaredOffsettedValueSum(int series, double offset) const;
  constexpr static double k_maxNumberOfBars = 10000.0;
  constexpr static float k_displayTopMarginRatio = 0.1f;
  constexpr static float k_displayRightMarginRatio = 0.04f;
  constexpr static int k_bottomMargin = 20;
  constexpr static float k_displayLeftMarginRatio = 0.04f;

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
  // Return the sumOfOccurrences, all frequencies must be integers
  int totalNormalProbabilityValues(int series) const;
  // Return the sorted element at cumulated population i+1
  double normalProbabilityValueAtIndex(int series, int i) const;
  // Return the z-score of the i-th sorted element
  double normalProbabilityResultAtIndex(int series, int i) const;

  // DoublePairStore
  void set(double f, int series, int i, int j) override;
  void memoizeValidSeries(int series) override;
  bool deleteValueAtIndex(int series, int i, int j) override;
  void deletePairOfSeriesAtIndex(int series, int j) override;
  void resetColumn(int series, int i) override;
  void deleteAllPairsOfSeries(int series) override;

private:
  double defaultValue(int series, int i, int j) const override;
  double sumOfValuesBetween(int series, double x1, double x2) const;
  /* Find the i-th distinct value (if i is -1, browse the entire series).
   * Retrieve the i-th value and the number distinct values encountered. */
  void countDistinctValuesUntil(int series, int i, double * value, int * distinctValues) const;
  double sortedElementAtCumulatedFrequency(int series, double k, bool createMiddleElement = false) const;
  double sortedElementAtCumulatedPopulation(int series, double population, bool createMiddleElement = false) const;
  int minIndex(double * bufferValues, int bufferLength) const;
  // Return the value index from its sorted index (a 0 sorted index is the min)
  size_t valueIndexAtSortedIndex(int series, int i) const;
  // Sort and memoize values indexes in increasing order.
  void buildSortedIndex(int series) const;
  // Histogram bars
  double m_barWidth;
  double m_firstDrawnBarAbscissa;
  // Sorted value indexes are memoized to save computation
  static_assert(k_maxNumberOfPairs <= SIZE_MAX, "k_maxNumberOfPairs is too large.");
  mutable size_t m_sortedIndex[k_numberOfSeries][k_maxNumberOfPairs];
  mutable bool m_sortedIndexValid[k_numberOfSeries];
};

typedef double (Store::*CalculPointer)(int) const;

}

#endif
