#ifndef STATISTICS_STORE_H
#define STATISTICS_STORE_H

#include "../shared/memoized_curve_view_range.h"
#include "../shared/float_pair_store.h"

namespace Statistics {

class Store : public Shared::MemoizedCurveViewRange, public Shared::FloatPairStore {
public:
  Store();
  uint32_t barChecksum();
  // Histogram bars
  double barWidth() const { return m_barWidth; }
  void setBarWidth(double barWidth);
  double firstDrawnBarAbscissa() const { return m_firstDrawnBarAbscissa; }
  void setFirstDrawnBarAbscissa(double firstDrawnBarAbscissa) { m_firstDrawnBarAbscissa = firstDrawnBarAbscissa;}
  double heightOfBarAtIndex(int series, int index);
  double heightOfBarAtValue(int series, double value);
  double startOfBarAtIndex(int series, int index);
  double endOfBarAtIndex(int series, int index);
  double numberOfBars(int series);
  // return true if the window has scrolled
  bool scrollToSelectedBarIndex(int series, int index);
  bool isEmpty();
  int numberOfNonEmptySeries();
  bool seriesIsEmpty(int i);

  // Calculation
  double sumOfOccurrences(int series);
  double maxValueForAllSeries();
  double minValueForAllSeries();
  double maxValue(int series);
  double minValue(int series);
  double range(int series);
  double mean(int series);
  double variance(int series);
  double standardDeviation(int series);
  double sampleStandardDeviation(int series);
  double firstQuartile(int series);
  double thirdQuartile(int series);
  double quartileRange(int series);
  double median(int series);
  double sum(int series);
  double squaredValueSum(int series);
  constexpr static double k_maxNumberOfBars = 10000.0;
  constexpr static float k_displayTopMarginRatio = 0.1f;
  constexpr static float k_displayRightMarginRatio = 0.04f;
  constexpr static int k_bottomMargin = 20;
  constexpr static float k_displayLeftMarginRatio = 0.04f;

private:
  double defaultValue(int series, int i, int j) override;
  double sumOfValuesBetween(int series, double x1, double x2);
  double sortedElementAtCumulatedFrequency(int series, double k, bool * exactElement = nullptr);
  double sortedElementAfter(int series, double k);
  int minIndex(double * bufferValues, int bufferLength);
  // Histogram bars
  double m_barWidth;
  double m_firstDrawnBarAbscissa;
};

typedef double (Store::*CalculPointer)(int);

}

#endif
