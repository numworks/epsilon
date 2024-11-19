#ifndef POINCARE_NUMERIC_STATISTICS_DATASET_COLUMN_H
#define POINCARE_NUMERIC_STATISTICS_DATASET_COLUMN_H

namespace Poincare::Internal {

class Tree;

template <typename T>
class DatasetColumn {
 public:
  virtual T valueAtIndex(int index) const = 0;
  virtual int length() const = 0;
};

template <typename T>
class ConstantDatasetColumn : public DatasetColumn<T> {
 public:
  ConstantDatasetColumn(T value, int length)
      : m_value(value), m_length(length) {}
  T valueAtIndex(int index) const override { return m_value; }
  int length() const override { return m_length; }

 private:
  T m_value;
  int m_length;
};

template <typename T>
class TreeDatasetColumn : public DatasetColumn<T> {
 public:
  // e must be an an approximated list tree.
  TreeDatasetColumn(const Tree* e);
  T valueAtIndex(int index) const override;
  int length() const override;

 private:
  const Tree* m_list;
};

}  // namespace Poincare::Internal

#endif
