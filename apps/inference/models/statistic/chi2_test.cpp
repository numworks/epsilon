#include "chi2_test.h"

#include <poincare/k_tree.h>

#include <cmath>

namespace Inference {

Chi2Test::Chi2Test() {
  m_hypothesis.m_alternative =
      Poincare::ComparisonJunior::Operator::Superior;  // Always higher
}

double Chi2Test::computeChi2() {
  double z = 0;
  int n = numberOfValuePairs();
  for (int i = 0; i < n; i++) {
    z += computeContribution(i);
  }
  return z;
}

double Chi2Test::computeContribution(int i) const {
  return std::pow(expectedValue(i) - observedValue(i), 2) / expectedValue(i);
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
