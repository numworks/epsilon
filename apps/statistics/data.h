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
  float valueAtIndex(int index);
  int sizeAtIndex(int index);
  void setValueAtIndex(float value, int index);
  void setSizeAtIndex(int size, int index);
  void deletePairAtIndex(int index);
  int sizeOfValuesBetweenBounds(float lowerBound, float upperBound) const;
  float xMin() override;
  // if the range of value is to wide, value max returns valueMin + 10
  float xMax() override;
  float yMin() override;
  float yMax() override;
  float xGridUnit() override;
  // TODO: decide the max number of elements after optimization
  constexpr static int k_maxNumberOfPairs = 500;
private:
  constexpr static int k_maxRangeValue = 320;
  int m_sizes[k_maxNumberOfPairs];
  float m_values[k_maxNumberOfPairs];
  int m_numberOfPairs;
};

}

#endif
