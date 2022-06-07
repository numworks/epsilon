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
  colorUnderCurve(ctx, rect, m_test->hypothesisParams()->comparisonOperator(), z);
  drawCurveAndAlphaStripes(ctx, rect, m_test->hypothesisParams()->comparisonOperator());
}

void TestCurveView::drawCurveAndAlphaStripes(KDContext * ctx, KDRect rect, HypothesisParams::ComparisonOperator op) const {
  if (op == HypothesisParams::ComparisonOperator::Different) {
    drawCurveAndAlphaStripes(ctx, rect, HypothesisParams::ComparisonOperator::Higher);
    drawCurveAndAlphaStripes(ctx, rect, HypothesisParams::ComparisonOperator::Lower);
    return;
  }

  float stripesStart, stripesEnd;
  float xAlpha = m_test->thresholdAbscissa(op);
  int pattern;
  if (op == HypothesisParams::ComparisonOperator::Higher) {
    stripesStart = xAlpha;
    stripesEnd = INFINITY;
    pattern = 0b1001;
  } else {
    assert(op == HypothesisParams::ComparisonOperator::Lower);
    stripesStart = -INFINITY;
    stripesEnd = xAlpha;
    pattern = 0b101;
  }

  assert(std::isfinite(xAlpha));
  drawCartesianCurve(ctx, rect, -INFINITY, INFINITY, evaluateAtAbscissa, m_test, nullptr, Palette::YellowDark, true, true, Palette::PurpleBright, stripesStart, stripesEnd, nullptr, false, nullptr, false, pattern);
}

void TestCurveView::colorUnderCurve(KDContext * ctx, KDRect rect, HypothesisParams::ComparisonOperator op, float z) const {
  if (op == HypothesisParams::ComparisonOperator::Different) {
    z = std::fabs(z);
    colorUnderCurve(ctx, rect, HypothesisParams::ComparisonOperator::Higher, z);
    colorUnderCurve(ctx, rect, HypothesisParams::ComparisonOperator::Lower, -z);
    return;
  }

  float xStart, xEnd;
  if (op == HypothesisParams::ComparisonOperator::Higher) {
    xStart = z;
    xEnd = curveViewRange()->xMax();
  } else {
    assert(op == HypothesisParams::ComparisonOperator::Lower);
    xStart = curveViewRange()->xMin();
    xEnd = z;
  }

  float xStep = pixelLengthToFloatLength(Axis::Horizontal, 1);
  for (float x = xStart; x <= xEnd; x += xStep) {
    float y = m_test->evaluateAtAbscissa(x);
    drawHorizontalOrVerticalSegment(ctx, rect, Axis::Vertical, x, 0.f, y, Palette::YellowDark);
  }
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
    absolute.invalidAllSizesPositionsAndBaselines();
    drawLabelAndGraduationAtPosition(ctx, -std::abs(z), Poincare::HorizontalLayout::Builder(Poincare::CodePointLayout::Builder('-', KDFont::SmallFont), absolute));
  } else {
    drawLabelAndGraduationAtPosition(ctx, z, m_test->testCriticalValueSymbol(KDFont::SmallFont));
  }
}

Poincare::Coordinate2D<float> TestCurveView::evaluateAtAbscissa(float x, void * model, void * context) {
  Test * test = static_cast<Test *>(model);
  return Poincare::Coordinate2D<float>(x, test->evaluateAtAbscissa(x));
}


}  // namespace Inference
