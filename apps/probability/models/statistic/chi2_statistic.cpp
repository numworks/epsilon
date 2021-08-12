#include "chi2_statistic.h"

#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

#include "probability/models/chi2_law.h"

namespace Probability {

Chi2Statistic::Chi2Statistic() : m_degreesOfFreedom(-1) {
  m_hypothesisParams.setOp(HypothesisParams::ComparisonOperator::Higher);  // Always higher
}

Poincare::Layout Chi2Statistic::testCriticalValueSymbol() {
  return Poincare::HorizontalLayout::Builder(
      Poincare::CodePointLayout::Builder(UCodePointGreekSmallLetterChi),
      Poincare::VerticalOffsetLayout::Builder(
          Poincare::CodePointLayout::Builder('2', KDFont::LargeFont),
          Poincare::VerticalOffsetLayoutNode::Position::Superscript));
}

float Chi2Statistic::normalizedDensityFunction(float x) const {
  assert(m_degreesOfFreedom > 0);
  return Chi2Law::EvaluateAtAbscissa(x, m_degreesOfFreedom);
}

float Chi2Statistic::cumulativeNormalizedDistributionFunction(float x) const {
  assert(m_degreesOfFreedom > 0);
  return Chi2Law::CumulativeDistributiveFunctionAtAbscissa(x, m_degreesOfFreedom);
}

float Chi2Statistic::cumulativeNormalizedInverseDistributionFunction(float proba) const {
  assert(m_degreesOfFreedom > 0);
  return Chi2Law::CumulativeDistributiveInverseForProbability(proba, m_degreesOfFreedom);
}

float Chi2Statistic::computeChi2() {
  float z = 0;
  int n = numberOfValuePairs();
  for (int i = 0; i < n; i++) {
    z += std::pow(expectedValue(i) - observedValue(i), 2) / expectedValue(i);
  }
  return z;
}

}  // namespace Probability
