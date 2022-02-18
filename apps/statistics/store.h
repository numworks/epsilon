#ifndef STATISTICS_STORE_H
#define STATISTICS_STORE_H

#include <apps/shared/memoized_curve_view_range.h>
#include <apps/shared/double_pair_store.h>

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
  double maxValueForAllSeries() const;
  double minValueForAllSeries() const;
  double maxValue(int series) const;
  double minValue(int series) const;
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

  // TODO : Factorize with upcoming changes in regression
  // TODO : Add tests
  void sortIndex(int series, int * sortedIndex, int startIndex, int endIndex) const;
  double cumulatedFrequencyAtSortedIndex(int series, int * sortedIndex, int index) const;

  // DoublePairStore
  void memoizeValidSeries(int series) override;
  bool deleteValueAtIndex(int series, int i, int j) override;

private:
  double defaultValue(int series, int i, int j) const override;
  double sumOfValuesBetween(int series, double x1, double x2) const;
  double sortedElementAtCumulatedFrequency(int series, double k, bool createMiddleElement = false) const;
  double sortedElementAtCumulatedPopulation(int series, double population, bool createMiddleElement = false) const;
  int minIndex(double * bufferValues, int bufferLength) const;
  // Histogram bars
  double m_barWidth;
  double m_firstDrawnBarAbscissa;
};

typedef double (Store::*CalculPointer)(int) const;

}

#endif
