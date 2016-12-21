#ifndef STATISTICS_DATA_H
#define STATISTICS_DATA_H

#include "../curve_view_window.h"

namespace Statistics {

class Data : public CurveViewWindow {
public:
  Data();
  // Delete the implicit copy constructor: the object is heavy
  Data(const Data&) = delete;
  int numberOfPairs() const;
  float binWidth();
  void setBinWidth(float binWidth);
  float valueAtIndex(int index);
  int sizeAtIndex(int index);
  void setValueAtIndex(float value, int index);
  void setSizeAtIndex(int size, int index);
  void deletePairAtIndex(int index);
  int sizeAtValue(float value);
  int totalSize();
  float minValue();
  void setMinValue(float minValue);
  float selectedBin();
  void initSelectedBin();
  void selectNextBinToward(int direction);
  float xMin() override;
  // if the range of value is to wide, value max returns valueMin + 10
  float xMax() override;
  float yMin() override;
  float yMax() override;
  float xGridUnit() override;
  // TODO: decide the max number of elements after optimization
  constexpr static int k_maxNumberOfPairs = 500;
private:
  constexpr static int k_maxRangeValue = 300;
  void computeTotalSize();
  void scrollToSelectedBin();
  void initWindowBounds();
  void computeYMax();
  void computeAbsoluteBoundValue();
  void updateAbsoluteBoundsAfterAdding(float value);
  void updateAbsoluteBoundsAfterDeleting(float value);
  int m_sizes[k_maxNumberOfPairs];
  float m_values[k_maxNumberOfPairs];
  int m_numberOfPairs;
  float m_binWidth;
  int m_totalSize;
  float m_selectedBin;
  // Absolute bounds of the data
  float m_minValue;
  float m_maxValue;
  // Window bounds of the data
  float m_xMin;
  float m_xMax;
  float m_yMax;
  float m_xGridUnit;
};

}

#endif
