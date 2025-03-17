#include "chi2_test.h"

#include <poincare/k_tree.h>

#include <cmath>

namespace Inference {

void Chi2Test::compute() {
  computeContributions();
  SignificanceTest::Results results =
      SignificanceTest::Chi2::Compute(&m_contributionsData, degreeOfFreedom());
  m_testCriticalValue = results.criticalValue;
  m_pValue = results.pValue;
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
