#ifndef APPS_DATA_H
#define APPS_DATA_H

class Data {
public:
  Data();
  virtual void deletePairAtIndex(int index) = 0;
  virtual void setXValueAtIndex(float f, int index) = 0;
  virtual void setYValueAtIndex(float f, int index) = 0;
  virtual float xValueAtIndex(int index) = 0;
  virtual float yValueAtIndex(int index) = 0;
  virtual void deleteAllXValues() = 0;
  virtual void deleteAllYValues() = 0;
  int numberOfPairs();
  // TODO: decide the max number of elements after optimization
  constexpr static int k_maxNumberOfPairs = 500;
protected:
  int m_numberOfPairs;
};

#endif
