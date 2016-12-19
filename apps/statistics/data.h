#ifndef STATISTICS_DATA_H
#define STATISTICS_DATA_H

namespace Statistics {

class Data {
public:
  Data();
  // Delete the implicit copy constructor: the object is heavy
  Data(const Data&) = delete;
  int numberOfPairs() const;
  float valueAtIndex(int index);
  int sizeAtIndex(int index);
  void setValueAtIndex(float value, int index);
  void setSizeAtIndex(int size, int index);
  // TODO: decide the max number of elements after optimization
  constexpr static int k_maxNumberOfPairs = 500;
private:
  int m_sizes[k_maxNumberOfPairs];
  float m_values[k_maxNumberOfPairs];
  int m_numberOfPairs;
};

}

#endif
