#include "test_curve_view.h"
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
  HypothesisParams::ComparisonOperator op = m_test->hypothesisParams()->comparisonOperator();
  colorUnderCurve(ctx, rect, op, z);
  drawCurveAndAlphaStripes(ctx, rect, op);
}

void TestCurveView::drawCurveAndAlphaStripes(KDContext * ctx, KDRect rect, HypothesisParams::ComparisonOperator op, double factor) const {
  if (op == HypothesisParams::ComparisonOperator::Different) {
    drawCurveAndAlphaStripes(ctx, rect, HypothesisParams::ComparisonOperator::Higher, 0.5);
    drawCurveAndAlphaStripes(ctx, rect, HypothesisParams::ComparisonOperator::Lower, 0.5);
    return;
  }

  float stripesStart, stripesEnd;
  int pattern;
  double xAlpha = m_test->thresholdAbscissa(op, factor);
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

  assert(!std::isnan(xAlpha));
  drawCartesianCurve(ctx, rect, -INFINITY, INFINITY, evaluateAtAbscissa, m_test, nullptr, Palette::YellowDark, CurveView::NoPotentialDiscontinuity, true, true, Palette::PurpleBright, stripesStart, stripesEnd, nullptr, false, nullptr, false, pattern);
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
    xStart = std::max(z, curveViewRange()->xMin());
    xEnd = curveViewRange()->xMax();
  } else {
    assert(op == HypothesisParams::ComparisonOperator::Lower);
    xStart = curveViewRange()->xMin();
    xEnd = std::min(z, curveViewRange()->xMax());
  }

  float xStep = pixelLengthToFloatLength(Axis::Horizontal, 1);
  for (float x = xStart; x < xEnd + xStep; x += xStep) {
    float y = m_test->evaluateAtAbscissa(x);
    drawHorizontalOrVerticalSegment(ctx, rect, Axis::Vertical, x, 0.f, y, Palette::YellowDark);
  }
}

void TestCurveView::drawLabelAndGraduationAtPosition(KDContext * ctx, float position, Poincare::Layout symbol) const {
  if ((curveViewRange()->xMin() <= position) && (position <= curveViewRange()->xMax())) {
    float verticalOrigin = std::round(floatToPixel(Axis::Vertical, 0.0f));
    KDCoordinate graduationPosition = drawGraduationAtPosition(ctx, position);

    KDPoint labelPosition = positionLabel(graduationPosition, verticalOrigin, symbol.layoutSize(k_font), RelativePosition::None, RelativePosition::Before);
    symbol.draw(ctx, labelPosition, k_font, KDColorBlack, k_backgroundColor);
  }
}

void TestCurveView::drawZLabelAndZGraduation(KDContext * ctx, float z) const {
  // Label
  if (z < curveViewRange()->xMin() || z > curveViewRange()->xMax()) {
    // z outside screen
    return;
  }
  if (m_test->hypothesisParams()->comparisonOperator() == HypothesisParams::ComparisonOperator::Different) {
    Poincare::AbsoluteValueLayout absolute = Poincare::AbsoluteValueLayout::Builder(m_test->testCriticalValueSymbol());
    drawLabelAndGraduationAtPosition(ctx, std::abs(z), absolute);
    absolute.invalidAllSizesPositionsAndBaselines();
    drawLabelAndGraduationAtPosition(ctx, -std::abs(z), Poincare::HorizontalLayout::Builder(Poincare::CodePointLayout::Builder('-'), absolute));
  } else {
    drawLabelAndGraduationAtPosition(ctx, z, m_test->testCriticalValueSymbol());
  }
}

Poincare::Coordinate2D<float> TestCurveView::evaluateAtAbscissa(float x, void * model, void * context) {
  Test * test = static_cast<Test *>(model);
  return Poincare::Coordinate2D<float>(x, test->evaluateAtAbscissa(x));
}


}  // namespace Inference
