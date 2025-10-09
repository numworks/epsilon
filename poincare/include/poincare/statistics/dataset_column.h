#pragma once

#include <omg/vector.h>

namespace Poincare {

template <typename T>
class DatasetColumn {
 public:
  virtual T valueAtIndex(int index) const = 0;
  virtual int length() const = 0;
};

template <typename T>
class VectorDatasetColumn : public DatasetColumn<T> {
 public:
  static constexpr size_t k_capacity = 100;

  VectorDatasetColumn(std::initializer_list<T> values) : m_vector{values} {}
  T valueAtIndex(int index) const override {
    assert(index >= 0);
    return m_vector[index];
  }
  int length() const override { return m_vector.size(); }

 private:
  OMG::StaticVector<T, k_capacity> m_vector;
};

}  // namespace Poincare
