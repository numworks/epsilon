#include "anova_test.h"

#include <poincare/statistics/inference.h>

namespace Inference {

void ANOVATest::compute() {
  m_results =
      Poincare::Inference::SignificanceTest::FTest::ComputeStatisticResults(
          m_groups.span());

  m_testCriticalValue = m_results.statistic;
  m_pValue = m_results.pValue;
}

bool ANOVATest::authorizedValueAtPosition(double p, int row, int column) const {
  return Poincare::Inference::SignificanceTest::FTest::IsObservedValueValid(p);
}

void ANOVATest::setValueAtPosition(double value, int row, int column) {
  assert(row >= 0 && column >= 0);
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

}  // namespace Inference
