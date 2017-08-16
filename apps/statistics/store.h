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
  float barWidth();
  void setBarWidth(float barWidth);
  float firstDrawnBarAbscissa();
  void setFirstDrawnBarAbscissa(float firstDrawnBarAbscissa);
  float heightOfBarAtIndex(int index);
  float heightOfBarAtValue(float value);
  float startOfBarAtIndex(int index);
  float endOfBarAtIndex(int index);
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
  double firstQuartile();
  double thirdQuartile();
  double quartileRange();
  double median();
  double sum();
  double squaredValueSum();
  constexpr static float k_maxNumberOfBars = 10000.0f;
  constexpr static float k_displayTopMarginRatio = 0.1f;
  constexpr static float k_displayRightMarginRatio = 0.04f;
  constexpr static float k_displayBottomMarginRatio = 0.4f;
  constexpr static float k_displayLeftMarginRatio = 0.04f;
private:
  double defaultValue(int i) override;
  float sumOfValuesBetween(float x1, float x2);
  double sortedElementNumber(int k);
  int minIndex(double * bufferValues, int bufferLength);
  // Histogram bars
  float m_barWidth;
  float m_firstDrawnBarAbscissa;
};

typedef double (Store::*CalculPointer)();

}

#endif
