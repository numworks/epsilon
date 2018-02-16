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
  double barWidth();
  void setBarWidth(double barWidth);
  double firstDrawnBarAbscissa();
  void setFirstDrawnBarAbscissa(double firstDrawnBarAbscissa);
  double heightOfBarAtIndex(int index);
  double heightOfBarAtValue(double value);
  double startOfBarAtIndex(int index);
  double endOfBarAtIndex(int index);
  double numberOfBars();
  // return true if the window has scrolled
  bool scrollToSelectedBarIndex(int index);

  // Calculation
  double sumOfOccurrences();
  double maxValue();
  double minValue();
  double range();
  double mean();
  double variance();
  double standardDeviation();
  double sampleStandardDeviation();
  double firstQuartile();
  double thirdQuartile();
  double quartileRange();
  double median();
  double sum();
  double squaredValueSum();
  constexpr static double k_maxNumberOfBars = 10000.0;
  constexpr static float k_displayTopMarginRatio = 0.1f;
  constexpr static float k_displayRightMarginRatio = 0.04f;
  constexpr static float k_displayBottomMarginRatio = 0.4f;
  constexpr static float k_displayLeftMarginRatio = 0.04f;
private:
  double defaultValue(int i, int j) override;
  double sumOfValuesBetween(double x1, double x2);
  double sortedElementNumber(int k);
  int minIndex(double * bufferValues, int bufferLength);
  // Histogram bars
  double m_barWidth;
  double m_firstDrawnBarAbscissa;
};

typedef double (Store::*CalculPointer)();

}

#endif
