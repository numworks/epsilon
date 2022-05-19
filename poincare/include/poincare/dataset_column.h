#ifndef POINCARE_DATASET_COLUMN_H
#define POINCARE_DATASET_COLUMN_H

namespace Poincare {

template<typename T>
class DatasetColumn {
public:
  virtual T valueAtIndex(int index) const = 0;
  virtual int length() const = 0;
};

template<typename T>
class ConstantDatasetColumn : public DatasetColumn<T> {
public:
  ConstantDatasetColumn(T value, int length) : m_value(value), m_length(length) {}
  T valueAtIndex(int index) const override { return m_value; }
  virtual int length() const override { return m_length; }
private:
  T m_value;
  int m_length;
};


}

#endif
