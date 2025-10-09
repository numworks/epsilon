#include <poincare/statistics/inference.h>
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

QUIZ_CASE(pcj_statistics_anova) {
  using namespace Poincare::Inference::SignificanceTest::FTest;

  static constexpr double k_precision = 1e-2;

  std::array<Values, 3> groups = {Values{3, 2, 4, 2, 1, 4, 2},
                                  Values{1, 3, 3, 2, 1, 4, 2, 3, 2},
                                  Values{3, 4, 2, 4, 5, 2, 4, 5}};

  StatisticResults statisticResults = ComputeStatisticResults(groups);
  assert_roughly_equal(statisticResults.statistic, 3.1776, k_precision);
  quiz_assert(statisticResults.between.degreesOfFreedom == 2);
  assert_roughly_equal(statisticResults.between.sumOfSquares, 7.74,
                       k_precision);
  assert_roughly_equal(statisticResults.between.meanSquares, 3.87, k_precision);
  quiz_assert(statisticResults.within.degreesOfFreedom == 21);
  assert_roughly_equal(statisticResults.within.sumOfSquares, 25.59,
                       k_precision);
  assert_roughly_equal(statisticResults.within.meanSquares, 1.22, k_precision);
}
