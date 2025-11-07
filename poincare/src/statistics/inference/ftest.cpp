
#include <omg/float.h>
#include <poincare/src/statistics/inference/significance_test.h>
#include <poincare/statistics/dataset_column.h>
#include <poincare/statistics/inference.h>

#include "poincare/comparison_operator.h"

namespace Poincare::Inference::SignificanceTest::FTest {

bool IsObservedValueValid(double value) { return !std::isnan(value); }

struct GlobalData {
  int nSamples;
  double mean;
};

GroupData ComputeGroupData(const Values& values) {
  Poincare::VectorDatasetColumn<Values::value_type, k_maxNumberOfGroupValues>
      vector{values};
  Poincare::StatisticsDataset<Values::value_type> dataset =
      Poincare::StatisticsDataset<Values::value_type>(&vector);
  return GroupData{.nSamples = dataset.datasetLength(),
                   .mean = dataset.mean(),
                   .stdDeviation = dataset.sampleStandardDeviation(),
                   .variance = dataset.variance(),
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
                 ComputeGroupData);
  return ComputeStatisticResults(
      std::span<GroupData>{groupDataArray.data(), groups.size()});
}

}  // namespace Poincare::Inference::SignificanceTest::FTest
