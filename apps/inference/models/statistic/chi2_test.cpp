#include "chi2_test.h"

#include <poincare/k_tree.h>

#include <cmath>

#include "poincare/comparison_operator.h"

namespace Inference {

void Chi2Test::compute() {
  computeContributions();
  m_testCriticalValue =
      SignificanceTest::Chi2::ComputeCriticalValue(&m_contributionsData);
  // Always use the superior operator for the chi2 test
  m_pValue = SignificanceTest::ComputePValue(
      statisticType(), Poincare::ComparisonJunior::Operator::Superior,
      m_testCriticalValue, degreeOfFreedom());
}

void Chi2Test::computeContributions() {
  SignificanceTest::Chi2::FillContributions(
      &m_observedValuesData, &m_expectedValuesData, &m_contributionsData);
}

float Chi2Test::computeXMax() const {
  return (1 + Shared::Inference::k_displayRightMarginRatio) *
         (m_degreesOfFreedom +
          Test::k_displayWidthToSTDRatio * std::sqrt(m_degreesOfFreedom));
}

float Chi2Test::computeXMin() const {
  return -Shared::Inference::k_displayLeftMarginRatio * computeXMax();
}

}  // namespace Inference
