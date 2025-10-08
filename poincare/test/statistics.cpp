#include <omg/vector.h>
#include <poincare/statistics/statistics_dataset.h>
#include <quiz.h>

#include <initializer_list>

#include "float_helper.h"

namespace Poincare {

template <typename T>
class VectorDatasetColumn : public DatasetColumn<T> {
 public:
  VectorDatasetColumn(std::initializer_list<T> values) : m_vector{values} {}
  T valueAtIndex(int index) const override {
    assert(index >= 0);
    return m_vector[index];
  }
  int length() const override { return m_vector.size(); }

 private:
  static constexpr size_t k_capacity = 100;
  OMG::StaticVector<T, k_capacity> m_vector;
};

}  // namespace Poincare

QUIZ_CASE(pcj_statistics_dataset) {
  Poincare::VectorDatasetColumn values{1.0, 2.0, 3.0, 4.0};
  Poincare::VectorDatasetColumn weights{1.0, 2.0, 1.0, 1.0};
  Poincare::StatisticsDataset<double> dataset(&values, &weights);
  quiz_assert(dataset.datasetLength() == 4);
  assert_roughly_equal(dataset.mean(), 2.4);
  assert_roughly_equal(dataset.weightedSum(), 12.0);
  assert_roughly_equal(dataset.squaredSum(), 34.0);
  // TODO: unit tests to be continued
}
