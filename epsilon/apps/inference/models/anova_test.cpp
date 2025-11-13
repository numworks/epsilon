#include "anova_test.h"

#include <poincare/statistics/inference.h>

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
  return m_groups.size() >= 2;
}

bool ANOVATest::authorizedValueAtPosition(double p, int row, int column) const {
  return Poincare::Inference::SignificanceTest::FTest::IsObservedValueValid(p);
}

void ANOVATest::setValueAtPosition(double value, int row, int column) {
  assert(row >= 0 && column >= 0);
  assert(!std::isnan(value));
  if (column > m_groups.size()) {
    return;
  }
  if (column < m_groups.size()) {
    GroupValues& groupValues = m_groups[column];
    if (row > groupValues.size()) {
      return;
    }
    if (row < groupValues.size()) {
      groupValues[row] = value;
      return;
    }
    assert(row == groupValues.size());
    groupValues.push(value);
    return;
  }
  assert(column == m_groups.size());
  if (row > 0) {
    // TODO: check that this acceptable
    return;
  }
  m_groups.push(GroupValues{value});
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
  if (groupValues.size() == 1) {
    assert(row == 0);
    // Remove group
    m_groups.removeAt(column);
    return true;
  }
  // Remove value from group
  groupValues.removeAt(row);
  return true;
}

}  // namespace Inference
