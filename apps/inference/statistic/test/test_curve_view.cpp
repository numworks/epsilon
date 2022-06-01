#include "test_curve_view.h"
#include "inference/text_helpers.h"
#include <poincare/absolute_value_layout.h>
#include <poincare/code_point_layout.h>
#include <algorithm>
#include <cmath>

using namespace Escher;

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
  drawAlphaStripes(ctx, rect, m_test->hypothesisParams()->comparisonOperator(), z);
  colorUnderCurve(ctx, rect, m_test->hypothesisParams()->comparisonOperator(), z);
}

void TestCurveView::drawAlphaStripes(KDContext * ctx, KDRect rect, HypothesisParams::ComparisonOperator op, float z) const {
  float xAlpha = m_test->thresholdAbscissa(op);
  float xStart, xEnd, xSolidEnd, xDir;
  int secondAreaIndex;
  switch (op) {
  case HypothesisParams::ComparisonOperator::Different:
    z = std::fabs(z);
    drawAlphaStripes(ctx, rect, HypothesisParams::ComparisonOperator::Higher, z);
    drawAlphaStripes(ctx, rect, HypothesisParams::ComparisonOperator::Lower, -z);
    return;
  case HypothesisParams::ComparisonOperator::Higher:
    xStart = curveViewRange()->xMax();
    xEnd = xAlpha;
    xSolidEnd = std::max(xEnd, z);
    xDir = -1.f;
    secondAreaIndex = 3;
    break;
  default:
    assert(op == HypothesisParams::ComparisonOperator::Lower);
    xStart = curveViewRange()->xMin();
    xEnd = xAlpha;
    xSolidEnd = std::min(xEnd, z);
    xDir = 1.f;
    secondAreaIndex = 2;
    break;
  }

  float xStep = pixelLengthToFloatLength(Axis::Horizontal, 1);
  for (float x = xStart; x * xDir < xEnd * xDir; x += xDir * xStep) {
    float y = m_test->evaluateAtAbscissa(x);
    if (x * xDir < xSolidEnd * xDir) {
      drawHorizontalOrVerticalSegment(ctx, rect, Axis::Vertical, x, 0.f, y, Palette::YellowDark);
    }
    drawHorizontalOrVerticalSegment(ctx, rect, Axis::Vertical, x, 0.f, y, Palette::GrayVeryDark, 1, 1, 0);
    drawHorizontalOrVerticalSegment(ctx, rect, Axis::Vertical, x, 0.f, y, Palette::GrayVeryDark, 1, 1, secondAreaIndex);
  }
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

  float xAlpha = m_test->thresholdAbscissa(op);
  float min = op == HypothesisParams::ComparisonOperator::Higher ? z : xAlpha;
  float max = op == HypothesisParams::ComparisonOperator::Higher ? xAlpha : z;
  drawCartesianCurve(
    ctx,
    rect,
    -INFINITY,
    INFINITY,
    evaluateAtAbscissa,
    m_test,
    nullptr,
    Palette::YellowDark,
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
