#include <poincare/statistics/statistics_dataset.h>
#include <quiz.h>

#include "float_helper.h"

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
