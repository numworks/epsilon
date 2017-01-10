#ifndef STATISTICS_STORE_H
#define STATISTICS_STORE_H

#include "../interactive_curve_view_range.h"
#include "../float_pair_store.h"

namespace Statistics {

class Store : public InteractiveCurveViewRange, public FloatPairStore {
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
  int numberOfBars();
  // return true if the window has scrolled
  bool scrollToSelectedBarIndex(int index);

  // Calculation
  float sumOfOccurrences();
  float maxValue();
  float minValue();
  float range();
  float mean();
  float variance();
  float standardDeviation();
  float firstQuartile();
  float thirdQuartile();
  float quartileRange();
  float median();
  float sum();
  float squaredValueSum();
private:
  float defaultValue(int i) override;
  float sumOfValuesBetween(float x1, float x2);
  float sortedElementNumber(int k);
  int minIndex(float * bufferValues, int bufferLength);
  // Histogram bars
  float m_barWidth;
  float m_firstDrawnBarAbscissa;
};

typedef float (Store::*CalculPointer)();

}

#endif
