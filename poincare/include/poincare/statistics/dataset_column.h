#pragma once

#include <omg/vector.h>

namespace Poincare {

template <typename T>
class DatasetColumn {
 public:
  virtual T valueAtIndex(int index) const = 0;
  virtual int length() const = 0;
};

template <typename T, size_t CAPACITY>
class VectorDatasetColumn : public DatasetColumn<T> {
 public:
  constexpr static size_t k_capacity = CAPACITY;

  using vector_type = OMG::StaticVector<T, k_capacity>;

  VectorDatasetColumn(std::initializer_list<T> values) : m_vector{values} {}
  explicit VectorDatasetColumn(const vector_type& vector) : m_vector{vector} {}

  T valueAtIndex(int index) const override {
    assert(index >= 0);
    return m_vector[index];
  }
  int length() const override { return m_vector.size(); }

 private:
  vector_type m_vector;
};

}  // namespace Poincare
