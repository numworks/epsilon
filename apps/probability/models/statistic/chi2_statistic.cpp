#include "chi2_statistic.h"

#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

#include "probability/models/chi2_law.h"

namespace Probability {

Chi2Statistic::Chi2Statistic() : m_degreesOfFreedom(-1) {
  m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Higher);  // Always higher
}

Poincare::Layout Chi2Statistic::testCriticalValueSymbol() {
  return Poincare::HorizontalLayout::Builder(
      Poincare::CodePointLayout::Builder(UCodePointGreekSmallLetterChi),
      Poincare::VerticalOffsetLayout::Builder(
          Poincare::CodePointLayout::Builder('2', KDFont::LargeFont),
          Poincare::VerticalOffsetLayoutNode::Position::Superscript));
}

bool Chi2Statistic::isValidParamAtIndex(int i, double p) {
  if (i == indexOfDegreeOfFreedom()) {
    return p == std::round(p) && p >= 1.0;
  }
  return Statistic::isValidParamAtIndex(i, p);
}

float Chi2Statistic::canonicalDensityFunction(float x) const {
  assert(m_degreesOfFreedom > 0);
  return Chi2Law::EvaluateAtAbscissa(x, static_cast<float>(m_degreesOfFreedom));
}

double Chi2Statistic::cumulativeNormalizedDistributionFunction(double x) const {
  assert(m_degreesOfFreedom > 0);
  return Chi2Law::CumulativeDistributiveFunctionAtAbscissa(x, m_degreesOfFreedom);
}

double Chi2Statistic::cumulativeNormalizedInverseDistributionFunction(double proba) const {
  assert(m_degreesOfFreedom > 0);
  return Chi2Law::CumulativeDistributiveInverseForProbability(proba, m_degreesOfFreedom);
}

float Chi2Statistic::xMin() const {
  return -k_displayLeftMarginRatio * xMax();
}

float Chi2Statistic::xMax() const {
  return (1 + k_displayRightMarginRatio) * (m_degreesOfFreedom + k_displayWidthToSTDRatio * std::sqrt(m_degreesOfFreedom));
}

float Chi2Statistic::yMax() const {
  // TODO factor with ChiSquareDistribution
  float max = m_degreesOfFreedom <= 2 ? 0.5 : canonicalDensityFunction(m_degreesOfFreedom - 1) * 1.2;
  return (1 + k_displayTopMarginRatio) * max;
}

double Chi2Statistic::computeChi2() {
  double z = 0;
  int n = numberOfValuePairs();
  for (int i = 0; i < n; i++) {
    z += std::pow(expectedValue(i) - observedValue(i), 2) / expectedValue(i);
  }
  return z;
}

}  // namespace Probability
