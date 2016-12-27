#ifndef STATISTICS_DATA_H
#define STATISTICS_DATA_H

#include "../curve_view_window.h"

namespace Statistics {

class Data : public CurveViewWindow {
public:
  Data();
  // Delete the implicit copy constructor: the object is heavy
  Data(const Data&) = delete;

  // Raw numeric data
  int numberOfPairs() const;
  float valueAtIndex(int index);
  int sizeAtIndex(int index);
  void setValueAtIndex(float value, int index);
  void setSizeAtIndex(int size, int index);
  void deletePairAtIndex(int index);
  int totalSize();

  // Histogram bars
  float barWidth();
  void setBarWidth(float barWidth);
  float firsBarAbscissa();
  void setFirsBarAbscissa(float firsBarAbscissa);
  int heightForBarAtValue(float value);
  float selectedBar();
  bool selectNextBarToward(int direction);

  //CurveViewWindow
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
  // TODO: decide the max number of elements after optimization
  constexpr static int k_maxNumberOfPairs = 500;
private:
  constexpr static int k_maxNumberOfBarsPerWindow = 300;
  float sumOfValuesBetween(float x1, float x2);
  bool scrollToSelectedBar();
  void initBarParameters();
  void initWindowParameters();
  float sortedElementNumber(int k);
  int minIndex(float * bufferValues, int bufferLength);
  // Raw numeric data
  int m_sizes[k_maxNumberOfPairs];
  float m_values[k_maxNumberOfPairs];
  int m_numberOfPairs;
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
