#include "anova_test.h"

#include <omg/unreachable.h>
#include <poincare/statistics/inference.h>

#include <cmath>

namespace Inference {

ANOVATest::GroupData GroupDataFromInputStatistics(
    const ANOVATest::InputStatisticsData& inputData) {
  int n = inputData.numberOfSamples();
  double mean = inputData.mean();
  double sampleStdDev = inputData.sampleStdDeviation();
  double sampleVariance = sampleStdDev * sampleStdDev;
  return ANOVATest::GroupData{
      .nSamples = n,
      .mean = mean,
      .sampleStdDeviation = sampleStdDev,
      .sampleVariance = sampleVariance,
      .sumOfValues = n * mean,
      // TODO: check this calculation
      .sumOfSquaredValues =
          n * mean * mean + (n - 1) * sampleVariance * sampleVariance};
}

double ANOVATest::InputStatisticsData::parameter(int parameterIndex) const {
  assert(parameterIndex >= 0 && parameterIndex < k_numberOfParameters);
  switch (parameterIndex) {
    case 0:
      return numberOfSamples();
    case 1:
      return mean();
    case 2:
      return sampleStdDeviation();
    default:
      OMG::unreachable();
  }
}

double& ANOVATest::InputStatisticsData::parameter(int parameterIndex) {
  assert(parameterIndex >= 0 && parameterIndex < k_numberOfParameters);
  switch (parameterIndex) {
    case 0:
      return m_nSamples;
    case 1:
      return m_mean;
    case 2:
      return m_sampleStdDeviation;
    default:
      OMG::unreachable();
  }
}

bool ANOVATest::InputStatisticsData::IsValueAcceptable(double value,
                                                       int parameterIndex) {
  assert(parameterIndex >= 0 && parameterIndex < k_numberOfParameters);
  assert(!std::isnan(value));
  if (parameterIndex == 0 && (value < 1.0 || std::floor(value) != value)) {
    return false;
  }
  // Sample standard deviation must be a positive number
  if (parameterIndex == 2 && (value < 0.0)) {
    return false;
  }
  return true;
}

void ANOVATest::InputStatisticsData::set(double value, int parameterIndex) {
  assert(IsValueAcceptable(value, parameterIndex));
  parameter(parameterIndex) = value;
}

void ANOVATest::InputStatisticsData::unsetStatisticParameter(
    int parameterIndex) {
  parameter(parameterIndex) = k_undefinedValue;
}

void ANOVATest::compute() {
  using namespace Poincare::Inference::SignificanceTest::FTest;

  m_groupData.resize(m_groups.size());
  std::transform(m_groups.begin(), m_groups.end(), m_groupData.begin(),
                 ComputeGroupData);
  assert(m_groupData.size() == m_groups.size());

  m_results = ComputeStatisticResults(m_groupData.span());

  m_testCriticalValue = m_results.statistic;
  m_pValue = m_results.pValue;
}

bool ANOVATest::validateInputs(int /* pageIndex */) {
  if (m_groups.size() < 2) {
    return false;
  }
  for (const GroupValues& group : m_groups) {
    for (double value : group) {
      if (std::isnan(value)) {
        return false;
      }
    }
  }
  return true;
}

void ANOVATest::setValueAtPosition(double value, int row, int column) {
  switch (m_inputMode) {
    case InputMode::Values:
      return setGroupValue(value, row, column);
    case InputMode::Statistics:
      return setStatisticParameter(value, row, column);
    default:
      OMG::unreachable();
  }
}

double ANOVATest::valueAtPosition(int row, int column) const {
  switch (m_inputMode) {
    case InputMode::Values:
      return groupValue(row, column);
    case InputMode::Statistics:
      return statisticParameter(row, column);
    default:
      OMG::unreachable();
  }
}

bool ANOVATest::deleteValueAtPosition(int row, int column) {
  switch (m_inputMode) {
    case InputMode::Values:
      return deleteGroupValue(row, column);
    case InputMode::Statistics:
      return deleteStatisticParameter(row, column);
    default:
      OMG::unreachable();
  }
}

void ANOVATest::deleteValuesInColumn(int column) {
  assert(column >= 0 && column < numberOfGroups());
  switch (m_inputMode) {
    case InputMode::Values:
      m_groups.removeAt(column);
      return;
    case InputMode::Statistics:
      m_inputStatistics.removeAt(column);
      return;
    default:
      OMG::unreachable();
  }
}

bool ANOVATest::authorizedValueAtPosition(double p, int row, int column) const {
  switch (m_inputMode) {
    case InputMode::Values:
      return Poincare::Inference::SignificanceTest::FTest::IsObservedValueValid(
          p);
    case InputMode::Statistics:
      return InputStatisticsData::IsValueAcceptable(p, row);
    default:
      OMG::unreachable();
  }
}

void ANOVATest::setGroupValue(double value, int valueIndex, int groupIndex) {
  assert(valueIndex >= 0 && groupIndex >= 0);
  /* setValueAtPosition is not used for deletion in ANOVATest, the passed value
   * must be a "real" value. */
  assert(!std::isnan(value));

  if (m_groups.size() <= groupIndex) {
    // Create one or more new groups
    for (int i = m_groups.size(); i <= groupIndex; ++i) {
      m_groups.push(GroupValues{});
    }
  }
  assert(m_groups.size() > groupIndex);

  GroupValues& groupValues = m_groups[groupIndex];
  if (groupValues.size() > valueIndex) {
    // Replace an existing value
    groupValues[valueIndex] = value;
    return;
  }
  // If needed, insert empty values in the group before the new value
  for (int i = groupValues.size(); i < valueIndex; ++i) {
    groupValues.push(k_undefinedValue);
  }
  assert(groupValues.size() == valueIndex);
  groupValues.push(value);
}

double ANOVATest::groupValue(int valueIndex, int groupIndex) const {
  assert(valueIndex >= 0 && groupIndex >= 0);
  if (groupIndex >= m_groups.size()) {
    return NAN;
  }
  const GroupValues& groupValues = m_groups[groupIndex];
  if (valueIndex >= groupValues.size()) {
    return NAN;
  }
  return groupValues[valueIndex];
}

bool ANOVATest::deleteGroupValue(int valueIndex, int groupIndex) {
  assert(valueIndex >= 0 && groupIndex >= 0);
  if (groupIndex >= m_groups.size()) {
    return false;
  }
  GroupValues& groupValues = m_groups[groupIndex];
  if (valueIndex >= groupValues.size()) {
    return false;
  }

  // Remove value from group
  groupValues.removeAt(valueIndex);
  // Remove potential empty values on top of the vector
  while ((groupValues.size() > 0) && (std::isnan(groupValues.last()))) {
    groupValues.pop();
  }

  if (groupValues.size() == 0) {
    // Remove group
    m_groups.removeAt(groupIndex);
  }

  return true;
}

void ANOVATest::setStatisticParameter(double value, int parameterIndex,
                                      int groupIndex) {
  assert(parameterIndex >= 0 &&
         parameterIndex < InputStatisticsData::k_numberOfParameters);
  assert(groupIndex >= 0);
  if (groupIndex >= numberOfGroups()) {
    for (int i = numberOfGroups(); i <= groupIndex; ++i) {
      m_inputStatistics.push(InputStatisticsData{});
    }
  }
  m_inputStatistics[groupIndex].set(value, parameterIndex);
}

double ANOVATest::statisticParameter(int parameterIndex, int groupIndex) const {
  assert(parameterIndex >= 0 && groupIndex >= 0);
  if (parameterIndex >= InputStatisticsData::k_numberOfParameters ||
      groupIndex >= m_inputStatistics.size()) {
    return k_undefinedValue;
  }
  return m_inputStatistics[groupIndex].parameter(parameterIndex);
}

bool ANOVATest::deleteStatisticParameter(int parameterIndex, int groupIndex) {
  assert(parameterIndex >= 0 && groupIndex >= 0);
  assert(parameterIndex < InputStatisticsData::k_numberOfParameters);
  if (groupIndex >= m_inputStatistics.size()) {
    return false;
  }
  InputStatisticsData& groupStatisticData = m_inputStatistics[groupIndex];
  groupStatisticData.unsetStatisticParameter(parameterIndex);
  if (groupStatisticData.isUnset()) {
    // Remove group
    m_inputStatistics.removeAt(groupIndex);
  }

  return true;
}

}  // namespace Inference
