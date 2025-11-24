
#include <omg/float.h>
#include <poincare/src/statistics/inference/significance_test.h>
#include <poincare/statistics/dataset_column.h>
#include <poincare/statistics/inference.h>

#include "poincare/comparison_operator.h"

namespace Poincare::Inference::SignificanceTest::FTest {

bool StatisticsData::IsValueAcceptable(double value, int parameterIndex) {
  assert(parameterIndex >= 0 && parameterIndex < k_numberOfParameters);
  assert(!std::isnan(value));
  /* There must be two samples per group at the very minimun for statistic
   * calculations to make any sense */
  if (parameterIndex == 0 && (value < 2.0 || std::floor(value) != value)) {
    return false;
  }
  // Sample standard deviation must be a positive number
  if (parameterIndex == 2 && (value < 0.0)) {
    return false;
  }
  return true;
}

void StatisticsData::set(double value, int parameterIndex) {
  switch (parameterIndex) {
    case 0:
      m_nSamples = value;
      return;
    case 1:
      m_mean = value;
      return;
    case 2:
      m_sampleStdDeviation = value;
      return;
    default:
      OMG::unreachable();
  }
}

bool IsObservedValueValid(double value) { return !std::isnan(value); }

struct GlobalData {
  int nSamples;
  double mean;
};

GroupData GroupDataFromStatisticsData(const StatisticsData& statisticsData) {
  double n = statisticsData.numberOfSamples();
  assert(n >= 0 && std::round(n) == n);
  double mean = statisticsData.mean();
  double sampleStdDev = statisticsData.sampleStdDeviation();
  double sampleVariance = sampleStdDev * sampleStdDev;
  return GroupData{
      .nSamples = static_cast<int>(n),
      .mean = mean,
      .sampleStdDeviation = sampleStdDev,
      .sampleVariance = sampleVariance,
      .sumOfValues = n * mean,
      /* The sum of squared value can be deduced from the mean and the sample
       * variance, knowing that mean = (1/n) * sum(x_i) and
       * sampleVariance = (1/(n-1)) * sum((x_i - mean)^2) */
      .sumOfSquaredValues = n * mean * mean + (n - 1) * sampleVariance};
}

GroupData GroupDataFromValues(const Values& values) {
  Poincare::VectorDatasetColumn<Values::value_type, k_maxNumberOfGroupValues>
      vector{values};
  Poincare::StatisticsDataset<Values::value_type> dataset =
      Poincare::StatisticsDataset<Values::value_type>(&vector);
  return GroupData{.nSamples = dataset.datasetLength(),
                   .mean = dataset.mean(),
                   .sampleStdDeviation = dataset.sampleStandardDeviation(),
                   .sampleVariance = dataset.sampleVariance(),
                   .sumOfValues = dataset.weightedSum(),
                   .sumOfSquaredValues = dataset.squaredSum()};
}

GlobalData ComputeGlobalData(std::span<const GroupData> groupDataList) {
  int totalNSamples = 0;
  double weightedMean = 0.0;
  for (const GroupData& groupData : groupDataList) {
    totalNSamples += groupData.nSamples;
    weightedMean += groupData.mean * groupData.nSamples;
  }
  assert(totalNSamples != 0);
  return GlobalData{.nSamples = totalNSamples,
                    .mean = weightedMean / totalNSamples};
}

static double SumOfSquaresBetween(std::span<const GroupData> groupDataList,
                                  double globalMean) {
  double sum = 0;
  for (const GroupData& groupData : groupDataList) {
    sum += groupData.nSamples * std::pow(groupData.mean - globalMean, 2.0);
  }
  return sum;
}

CalculatedValues ComputeBetween(const GlobalData& globalData,
                                std::span<const GroupData> groupDataList) {
  double SSB = SumOfSquaresBetween(groupDataList, globalData.mean);
  int nGroups = static_cast<int>(groupDataList.size());
  int df = nGroups - 1;
  assert(df != 0);
  return CalculatedValues{
      .sumOfSquares = SSB, .degreesOfFreedom = df, .meanSquares = SSB / df};
}

static double SumOfSquaresWithin(std::span<const GroupData> groupDataList) {
  double sum = 0;
  for (const GroupData& groupData : groupDataList) {
    sum += groupData.sumOfSquaredValues -
           std::pow(groupData.sumOfValues, 2.0) / groupData.nSamples;
  }
  return sum;
}

CalculatedValues ComputeWithin(const GlobalData& globalData,
                               std::span<const GroupData> groupDataList) {
  double SSW = SumOfSquaresWithin(groupDataList);
  int nGroups = static_cast<int>(groupDataList.size());
  int df = globalData.nSamples - nGroups;
  assert(df != 0);
  return CalculatedValues{
      .sumOfSquares = SSW, .degreesOfFreedom = df, .meanSquares = SSW / df};
}

double Statistic(const CalculatedValues& withinValues,
                 const CalculatedValues& betweenValues) {
  assert(std::abs(withinValues.meanSquares) > OMG::Float::EpsilonLax<double>());
  return betweenValues.meanSquares / withinValues.meanSquares;
}

StatisticResults ComputeStatisticResults(std::span<const GroupData> groupData) {
  GlobalData globalData = ComputeGlobalData(groupData);
  CalculatedValues within = ComputeWithin(globalData, groupData);
  CalculatedValues between = ComputeBetween(globalData, groupData);

  double statistic = Statistic(within, between);
  double pValue = Internal::Inference::SignificanceTest::ComputePValue(
      StatisticType::F, Comparison::Operator::Superior, statistic,
      between.degreesOfFreedom, within.degreesOfFreedom);
  return StatisticResults{.statistic = statistic,
                          .pValue = pValue,
                          .between = between,
                          .within = within};
}

StatisticResults ComputeStatisticResults(std::span<const Values> groups) {
  assert(groups.size() <= k_maxNumberOfGroups);
  std::array<GroupData, k_maxNumberOfGroups> groupDataArray;
  std::transform(groups.begin(), groups.end(), groupDataArray.begin(),
                 GroupDataFromValues);
  return ComputeStatisticResults(
      std::span<GroupData>{groupDataArray.data(), groups.size()});
}

StatisticResults ComputeStatisticResults(
    std::span<const StatisticsData> groupStatistics) {
  assert(groupStatistics.size() <= k_maxNumberOfGroups);
  std::array<GroupData, k_maxNumberOfGroups> groupDataArray;
  std::transform(groupStatistics.begin(), groupStatistics.end(),
                 groupDataArray.begin(), GroupDataFromStatisticsData);
  return ComputeStatisticResults(
      std::span<GroupData>{groupDataArray.data(), groupStatistics.size()});
}

}  // namespace Poincare::Inference::SignificanceTest::FTest
