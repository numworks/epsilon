#ifndef STATISTICS_STORE_H
#define STATISTICS_STORE_H

#include "../curve_view_window.h"
#include "../float_pair_store.h"

namespace Statistics {

class Store : public CurveViewWindow, public FloatPairStore {
public:
  Store();

  // Histogram bars
  void initBarParameters();
  float barWidth();
  void setBarWidth(float barWidth);
  float firsBarAbscissa();
  void setFirsBarAbscissa(float firsBarAbscissa);
  int heightForBarAtValue(float value);
  float selectedBar();
  bool selectNextBarToward(int direction);

  //CurveViewWindow
  void initWindowParameters();
  float xMin() override;
  // if the range of value is to wide compared to the bar width, value max is capped
  float xMax() override;
  float yMin() override;
  float yMax() override;
  float xGridUnit() override;

  // Calculation
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
  constexpr static int k_maxNumberOfBarsPerWindow = 300;
  float defaultValue(int i) override;
  float sumOfValuesBetween(float x1, float x2);
  bool scrollToSelectedBar();
  float sortedElementNumber(int k);
  int minIndex(float * bufferValues, int bufferLength);
  float closestMiddleBarTo(float f);
  // Histogram bars
  float m_barWidth;
  float m_selectedBar;
  float m_firstBarAbscissa;
  // Window bounds of the data
  float m_xMin;
  float m_xMax;
  float m_yMax;
  float m_xGridUnit;
};

}

#endif
