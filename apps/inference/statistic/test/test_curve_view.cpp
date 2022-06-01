#include "test_curve_view.h"
#include <poincare/absolute_value_layout.h>
#include <poincare/code_point_layout.h>
#include "inference/text_helpers.h"
#include <cmath>

namespace Inference {

void TestCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  StatisticCurveView::drawRect(ctx, rect);
  drawTest(ctx, rect);
}

bool TestCurveView::shouldDrawLabelAtPosition(float labelValue) const {
  float margin = k_marginsAroundZLabel / static_cast<float>(bounds().width()) * (curveViewRange()->xMax() - curveViewRange()->xMin());
  float z = static_cast<float>(m_test->testCriticalValue());
  bool res = labelValue >= z + margin || labelValue <= z - margin;
  if (m_test->hypothesisParams()->comparisonOperator() == HypothesisParams::ComparisonOperator::Different) {
    res = res && (labelValue >= -z + margin || labelValue <= -z - margin);
  }
  return res;
}

void TestCurveView::drawTest(KDContext * ctx, KDRect rect) const {
  float z = static_cast<float>(m_test->testCriticalValue());

  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, false, false, 0, k_backgroundColor);
  drawZLabelAndZGraduation(ctx, z);
  colorUnderCurve(ctx, rect, m_test->hypothesisParams()->comparisonOperator(), z);
}

void TestCurveView::colorUnderCurve(KDContext * ctx, KDRect rect, HypothesisParams::ComparisonOperator op, float z) const {
  if (op == HypothesisParams::ComparisonOperator::Different) {
    // Recurse for both colors
    z = std::fabs(z);
    // TODO optimize ?
    colorUnderCurve(ctx, rect, HypothesisParams::ComparisonOperator::Higher, z);
    colorUnderCurve(ctx, rect, HypothesisParams::ComparisonOperator::Lower, -z);
    return;
  }

  float min = op == HypothesisParams::ComparisonOperator::Higher ? z : -INFINITY;
  float max = op == HypothesisParams::ComparisonOperator::Higher ? INFINITY : z;
  drawCartesianCurve(
    ctx,
    rect,
    -INFINITY,
    INFINITY,
    evaluateAtAbscissa,
    m_test,
    nullptr,
    Escher::Palette::YellowDark,
    true,
    true,
    min,
    max);
}

void TestCurveView::drawLabelAndGraduationAtPosition(KDContext * ctx, float position, Poincare::Layout symbol) const {
  if ((curveViewRange()->xMin() <= position) && (position <= curveViewRange()->xMax())) {
    float verticalOrigin = std::round(floatToPixel(Axis::Vertical, 0.0f));
    KDCoordinate graduationPosition = drawGraduationAtPosition(ctx, position);

    KDPoint labelPosition = positionLabel(graduationPosition, verticalOrigin, symbol.layoutSize(), RelativePosition::None, RelativePosition::Before);
    symbol.draw(ctx, labelPosition, KDColorBlack, k_backgroundColor);
  }
}

void TestCurveView::drawZLabelAndZGraduation(KDContext * ctx, float z) const {
  // Label
  if (z < curveViewRange()->xMin() || z > curveViewRange()->xMax()) {
    // z outside screen
    return;
  }
  if (m_test->hypothesisParams()->comparisonOperator() == HypothesisParams::ComparisonOperator::Different) {
    Poincare::AbsoluteValueLayout absolute = Poincare::AbsoluteValueLayout::Builder(m_test->testCriticalValueSymbol(KDFont::SmallFont));
    drawLabelAndGraduationAtPosition(ctx, std::abs(z), absolute);
    drawLabelAndGraduationAtPosition(ctx, -std::abs(z), Poincare::HorizontalLayout::Builder(Poincare::CodePointLayout::Builder('-'), absolute));
  } else {
    drawLabelAndGraduationAtPosition(ctx, z, m_test->testCriticalValueSymbol(KDFont::SmallFont));
  }
}

Poincare::Coordinate2D<float> TestCurveView::evaluateAtAbscissa(float x, void * model, void * context) {
  Test * test = static_cast<Test *>(model);
  return Poincare::Coordinate2D<float>(x, test->evaluateAtAbscissa(x));
}


}  // namespace Inference
