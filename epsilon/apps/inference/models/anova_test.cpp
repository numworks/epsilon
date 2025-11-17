#include "anova_test.h"

#include <poincare/statistics/inference.h>

#include <cmath>

namespace Inference {

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

bool ANOVATest::authorizedValueAtPosition(double p, int row, int column) const {
  return Poincare::Inference::SignificanceTest::FTest::IsObservedValueValid(p);
}

void ANOVATest::setValueAtPosition(double value, int row, int column) {
  assert(row >= 0 && column >= 0);
  /* setValueAtPosition is not used for deletion in ANOVATest, the passed value
   * must be a "real" value. */
  assert(!std::isnan(value));

  if (m_groups.size() <= column) {
    // Create one or more new groups
    for (int i = m_groups.size(); i <= column; ++i) {
      m_groups.push(GroupValues{});
    }
  }
  assert(m_groups.size() > column);

  GroupValues& groupValues = m_groups[column];
  if (groupValues.size() > row) {
    // Replace an existing value
    groupValues[row] = value;
    return;
  }
  // If needed, insert empty values in the group before the new value
  for (int i = groupValues.size(); i < row; ++i) {
    groupValues.push(k_undefinedValue);
  }
  assert(groupValues.size() == row);
  groupValues.push(value);
}

double ANOVATest::valueAtPosition(int row, int column) const {
  assert(row >= 0 && column >= 0);
  if (column >= m_groups.size()) {
    return NAN;
  }
  const GroupValues& groupValues = m_groups[column];
  if (row >= groupValues.size()) {
    return NAN;
  }
  return groupValues[row];
}

bool ANOVATest::deleteValueAtPosition(int row, int column) {
  assert(row >= 0 && column >= 0);
  if (column >= m_groups.size()) {
    return false;
  }
  GroupValues& groupValues = m_groups[column];
  if (row >= groupValues.size()) {
    return false;
  }

  // Remove value from group
  groupValues.removeAt(row);
  // Remove potential empty values on top of the vector
  while ((groupValues.size() > 0) && (std::isnan(groupValues.last()))) {
    groupValues.pop();
  }

  if (groupValues.size() == 0) {
    // Remove group
    m_groups.removeAt(column);
  }

  return true;
}

void ANOVATest::deleteValuesInColumn(int column) {
  assert(column >= 0 && column < m_groups.size());
  m_groups.removeAt(column);
}

}  // namespace Inference
