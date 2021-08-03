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

float Chi2Statistic::normedDensityFunction(float x) {
  return Chi2Law::EvaluateAtAbscissa(x, m_degreesOfFreedom);
}

float Chi2Statistic::_z() {
  float z = 0;
  int n = numberOfValuePairs();
  for (int i = 0; i < n; i++) {
    z += std::pow(expectedValue(i) - observedValue(i), 2) / expectedValue(i);
  }
  return z;
}

float Chi2Statistic::_zAlpha(float degreesOfFreedom, float significanceLevel) {
  return Chi2Law::CumulativeDistributiveInverseForProbability(1 - significanceLevel,
                                                              degreesOfFreedom);
}

float Chi2Statistic::_pVal(float degreesOfFreedom, float z) {
  return 1 - Chi2Law::CumulativeDistributiveFunctionAtAbscissa<float>(z, degreesOfFreedom);
}

}  // namespace Probability
