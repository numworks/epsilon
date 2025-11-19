#include <poincare/statistics/inference.h>
#include <poincare/statistics/statistics_dataset.h>
#include <quiz.h>

#include <cmath>

#include "float_helper.h"

QUIZ_CASE(pcj_statistics_dataset) {
  using Dataset = Poincare::VectorDatasetColumn<double, 10>;
  Dataset values{1.0, 2.0, 3.0, 4.0};
  Dataset weights{1.0, 2.0, 1.0, 1.0};
  Poincare::StatisticsDataset<double> dataset(&values, &weights);
  quiz_assert(dataset.datasetLength() == 4);
  assert_roughly_equal(dataset.mean(), 2.4);
  assert_roughly_equal(dataset.weightedSum(), 12.0);
  assert_roughly_equal(dataset.squaredSum(), 34.0);
  // TODO: unit tests to be continued
}

static constexpr double k_precision1 = 1e-3;
static constexpr double k_precision2 = 1e-2;

QUIZ_CASE(pcj_statistics_anova_from_values) {
  using namespace Poincare::Inference::SignificanceTest::FTest;

  std::array<Values, 3> groups = {Values{3, 2, 4, 2, 1, 4, 2},
                                  Values{1, 3, 3, 2, 1, 4, 2, 3, 2},
                                  Values{3, 4, 2, 4, 5, 2, 4, 5}};

  StatisticResults statisticResults =
      ComputeStatisticResults(std::span<const Values>(groups));
  assert_roughly_equal(statisticResults.statistic, 3.1776, k_precision1);
  assert_roughly_equal(statisticResults.pValue, 0.0623, k_precision1);

  quiz_assert(statisticResults.between.degreesOfFreedom == 2);
  assert_roughly_equal(statisticResults.between.sumOfSquares, 7.74,
                       k_precision2);
  assert_roughly_equal(statisticResults.between.meanSquares, 3.87,
                       k_precision2);
  quiz_assert(statisticResults.within.degreesOfFreedom == 21);
  assert_roughly_equal(statisticResults.within.sumOfSquares, 25.59,
                       k_precision2);
  assert_roughly_equal(statisticResults.within.meanSquares, 1.22, k_precision2);
}

QUIZ_CASE(pcj_statistics_anova_from_statistics) {
  using namespace Poincare::Inference::SignificanceTest::FTest;

  // Same group data as previous test case

  std::array<StatisticsData, 3> groupStatistics = {
      StatisticsData(7, 2.57149, 1.133893), StatisticsData(9, 2.333333, 1),
      StatisticsData(8, 3.625, 1.187735)};

  std::array<GroupData, k_maxNumberOfGroups> groupDataArray;
  std::transform(groupStatistics.begin(), groupStatistics.end(),
                 groupDataArray.begin(), GroupDataFromStatisticsData);

  StatisticResults statisticResults =
      ComputeStatisticResults(std::span<const StatisticsData>(groupStatistics));

  quiz_assert(statisticResults.between.degreesOfFreedom == 2);
  assert_roughly_equal(statisticResults.between.sumOfSquares, 7.74,
                       k_precision2);
  assert_roughly_equal(statisticResults.between.meanSquares, 3.87,
                       k_precision2);
  quiz_assert(statisticResults.within.degreesOfFreedom == 21);
  assert_roughly_equal(statisticResults.within.sumOfSquares, 25.59,
                       k_precision2);
  assert_roughly_equal(statisticResults.within.meanSquares, 1.22, k_precision2);

  assert_roughly_equal(statisticResults.statistic, 3.1776, k_precision1);
  assert_roughly_equal(statisticResults.pValue, 0.0623, k_precision1);
}
