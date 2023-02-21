#include "test_curve_view.h"

#include <poincare/absolute_value_layout.h>
#include <poincare/code_point_layout.h>

#include <algorithm>
#include <cmath>

using namespace Escher;
using namespace Poincare;
using namespace Shared;

namespace Inference {

// TestPlotPolicy

void TestPlotPolicy::drawPlot(const AbstractPlotView *plotView, KDContext *ctx,
                              KDRect rect) const {
  float z = static_cast<float>(m_test->testCriticalValue());
  ComparisonNode::OperatorType op =
      m_test->hypothesisParams()->comparisonOperator();
  drawZLabelAndZGraduation(plotView, ctx, rect, z, op);
  drawTestCurve(plotView, ctx, rect, z, op);
}

void TestPlotPolicy::drawZLabelAndZGraduation(
    const AbstractPlotView *plotView, KDContext *ctx, KDRect rect, float z,
    ComparisonNode::OperatorType op) const {
  if (op == Poincare::ComparisonNode::OperatorType::NotEqual) {
    AbsoluteValueLayout absolute = Poincare::AbsoluteValueLayout::Builder(
        m_test->testCriticalValueSymbol());
    drawLabelAndGraduation(plotView, ctx, rect, std::abs(z), absolute);
    absolute.invalidAllSizesPositionsAndBaselines();
    drawLabelAndGraduation(
        plotView, ctx, rect, -std::abs(z),
        HorizontalLayout::Builder(CodePointLayout::Builder('-'), absolute));
  } else {
    drawLabelAndGraduation(plotView, ctx, rect, z,
                           m_test->testCriticalValueSymbol());
  }
}

void TestPlotPolicy::drawLabelAndGraduation(const AbstractPlotView *plotView,
                                            KDContext *ctx, KDRect rect,
                                            float x, Layout layout) const {
  plotView->drawTick(ctx, rect, AbstractPlotView::Axis::Horizontal, x);
  plotView->drawLayout(ctx, rect, layout, Coordinate2D<float>(x, 0.f),
                       AbstractPlotView::RelativePosition::There,
                       AbstractPlotView::RelativePosition::After, KDColorBlack);
}

static Coordinate2D<float> evaluate(float x, void *model, void *) {
  Test *test = reinterpret_cast<Test *>(model);
  return Coordinate2D<float>(x, test->evaluateAtAbscissa(x));
}

static Coordinate2D<float> evaluateZero(float, void *, void *) {
  return Coordinate2D<float>(0.f, 0.f);
}

void TestPlotPolicy::drawTestCurve(const Shared::AbstractPlotView *plotView,
                                   KDContext *ctx, KDRect rect, float z,
                                   ComparisonNode::OperatorType op,
                                   double factor) const {
  if (op == Poincare::ComparisonNode::OperatorType::NotEqual) {
    z = std::fabs(z);
    drawTestCurve(plotView, ctx, rect, z,
                  Poincare::ComparisonNode::OperatorType::Superior, 0.5);
    drawTestCurve(plotView, ctx, rect, -z,
                  Poincare::ComparisonNode::OperatorType::Inferior, 0.5);
    return;
  }

  CurveViewRange *range = plotView->range();
  float xAlpha = m_test->thresholdAbscissa(op, factor);
  float clampedXAlpha = std::clamp(xAlpha, range->xMin(), range->xMax());
  float clampedZ = std::clamp(z, range->xMin(), range->xMax());
  /* We can draw one of the two following combination of patterns (reversed for
   * ComparisonOperator::Lower):
   * - No pattern | Stripes   | Stripes&Highlight
   *            xAlpha        z
   *
   * - No pattern | Highlight | Stripes&Highlight
   *              z         xAlpha
   */
  Pattern patternBoth, patternSingle;
  float bothStart, bothEnd, singleStart, singleEnd, singleCurveStart,
      singleCurveEnd;
  if (op == Poincare::ComparisonNode::OperatorType::Superior) {
    patternBoth = Pattern(true, false, false, true, Palette::PurpleBright,
                          Palette::YellowDark);
    singleCurveStart = range->xMin();
    if (z < xAlpha) {
      singleStart = clampedZ;
      patternSingle = Pattern(Palette::YellowDark);
      bothStart = clampedXAlpha;
    } else {
      singleStart = clampedXAlpha;
      patternSingle = Pattern(true, false, false, true, Palette::PurpleBright);
      bothStart = clampedZ;
    }
    singleStart -= plotView->pixelWidth();
    singleCurveEnd = singleEnd = bothStart;
    bothEnd = range->xMax();
  } else {
    assert(op == Poincare::ComparisonNode::OperatorType::Inferior);
    patternBoth = Pattern(true, false, true, false, Palette::PurpleBright,
                          Palette::YellowDark);
    bothStart = range->xMin();
    if (z < xAlpha) {
      bothEnd = clampedZ;
      patternSingle = Pattern(true, false, true, false, Palette::PurpleBright);
      singleEnd = clampedXAlpha;
    } else {
      bothEnd = clampedXAlpha;
      patternSingle = Pattern(Palette::YellowDark);
      singleEnd = clampedZ;
    }
    singleCurveStart = singleStart = bothEnd;
    singleCurveEnd = range->xMax();
  }

  {
    CurveDrawing plot(Curve2D(evaluate, m_test), nullptr, bothStart, bothEnd,
                      plotView->pixelWidth(), Palette::YellowDark);
    plot.setPatternOptions(patternBoth, bothStart, bothEnd,
                           Curve2D(evaluateZero), Curve2D(), false);
    plot.draw(plotView, ctx, rect);
  }
  {
    CurveDrawing plot(Curve2D(evaluate, m_test), nullptr, singleCurveStart,
                      singleCurveEnd, plotView->pixelWidth(),
                      Palette::YellowDark);
    plot.setPatternOptions(patternSingle, singleStart, singleEnd,
                           Curve2D(evaluateZero), Curve2D(), false);
    plot.draw(plotView, ctx, rect);
  }
}

// TestXAxis

void TestXAxis::drawLabel(int i, float t, const AbstractPlotView *plotView,
                          KDContext *ctx, KDRect rect,
                          AbstractPlotView::Axis axis, KDColor color) const {
  assert(axis == AbstractPlotView::Axis::Horizontal);
  const TestCurveView *testCurveView =
      static_cast<const TestCurveView *>(plotView);
  float z = testCurveView->test()->testCriticalValue();
  KDCoordinate labelWidth =
      KDFont::Font(AbstractPlotView::k_font)->stringSize(label(i)).width();
  KDCoordinate zWidth = KDFont::GlyphSize(AbstractPlotView::k_font).width() *
                        TestPlotPolicy::k_zLabelMaxGlyphLength;
  if (std::fabs(z - t) >
      plotView->pixelWidth() * 0.5f * (labelWidth + zWidth)) {
    /* Label does not overlap with z label, it can be drawn */
    PlotPolicy::HorizontalLabeledAxis::drawLabel(i, t, plotView, ctx, rect,
                                                 axis);
  }
}

// TestCurveView

TestCurveView::TestCurveView(Test *test) : PlotView(test) {
  // TestPlotPolicy
  m_test = test;
}

}  // namespace Inference
