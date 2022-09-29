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

void TestPlotPolicy::drawPlot(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  float z = static_cast<float>(m_test->testCriticalValue());
  HypothesisParams::ComparisonOperator op = m_test->hypothesisParams()->comparisonOperator();
  drawZLabelAndZGraduation(plotView, ctx, rect, z, op);
  drawTestCurve(plotView, ctx, rect, z, op);
}

void TestPlotPolicy::drawZLabelAndZGraduation(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float z, HypothesisParams::ComparisonOperator op) const {
  if (op == HypothesisParams::ComparisonOperator::Different) {
    AbsoluteValueLayout absolute = Poincare::AbsoluteValueLayout::Builder(m_test->testCriticalValueSymbol());
    drawLabelAndGraduation(plotView, ctx, rect, std::abs(z), absolute);
    absolute.invalidAllSizesPositionsAndBaselines();
    drawLabelAndGraduation(plotView, ctx, rect, -std::abs(z), HorizontalLayout::Builder(CodePointLayout::Builder('-'), absolute));
  } else {
    drawLabelAndGraduation(plotView, ctx, rect, z, m_test->testCriticalValueSymbol());
  }
}

void TestPlotPolicy::drawLabelAndGraduation(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float x, Layout layout) const {
  float gradLength = PlotPolicy::SimpleAxis::k_labelGraduationHalfLength * plotView->pixelHeight();
  plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Vertical, x, gradLength, -gradLength, KDColorBlack);
  plotView->drawLayout(ctx, rect, layout, Coordinate2D<float>(x, 0.f), AbstractPlotView::RelativePosition::There, AbstractPlotView::RelativePosition::After, KDColorBlack);
}

static Coordinate2D<float> evaluate(float x, void * model, void *) {
  Test * test = reinterpret_cast<Test *>(model);
  return Coordinate2D<float>(x, test->evaluateAtAbscissa(x));
}

static Coordinate2D<float> evaluateZero(float, void *, void *) { return Coordinate2D<float>(0.f, 0.f); }

void TestPlotPolicy::drawTestCurve(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float z, HypothesisParams::ComparisonOperator op, double factor) const {
  if (op == HypothesisParams::ComparisonOperator::Different) {
    drawTestCurve(plotView,ctx, rect, z, HypothesisParams::ComparisonOperator::Higher, 0.5);
    drawTestCurve(plotView,ctx, rect, -z, HypothesisParams::ComparisonOperator::Lower, 0.5);
    return;
  }

  CurveViewRange * range = plotView->range();
  double xAlpha = m_test->thresholdAbscissa(op, factor);
  /* We can draw one of the two following combination of patterns (reversed for
   * ComparisonOperator::Lower):
   * - No pattern | Stripes   | Stripes&Highlight
   *            xAlpha        z
   *
   * - No pattern | Highlight | Stripes&Highlight
   *              z         xAlpha
   */
  Pattern patternBoth, patternSingle;
  float bothStart, bothEnd, singleStart, singleEnd, singleCurveStart, singleCurveEnd;
  if (op == HypothesisParams::ComparisonOperator::Higher) {
    patternBoth = Pattern(true, false, false, true, Palette::PurpleBright, Palette::YellowDark);
    singleCurveStart = range->xMin();
    if (z < xAlpha) {
      singleStart = z;
      patternSingle = Pattern(Palette::YellowDark);
      bothStart = xAlpha;
    } else {
      singleStart = xAlpha;
      patternSingle = Pattern(true, false, false, true, Palette::PurpleBright);
      bothStart = z;
    }
    singleStart -= plotView->pixelWidth();
    singleCurveEnd = singleEnd = bothStart;
    bothEnd = range->xMax();
  } else {
    patternBoth = Pattern(true, false, true, false, Palette::PurpleBright, Palette::YellowDark);
    bothStart = range->xMin();
    if (z < xAlpha) {
      bothEnd = z;
      patternSingle = Pattern(true, false, false, true, Palette::PurpleBright);
      singleEnd = xAlpha;
    } else {
      bothEnd = xAlpha;
      patternSingle = Pattern(Palette::YellowDark);
      singleEnd = z;
    }
    singleCurveStart = singleStart = bothEnd;
    singleCurveEnd = range->xMax();
  }

  {
    CurveDrawing plot(evaluate, m_test, nullptr, bothStart, bothEnd, plotView->pixelWidth(), Palette::YellowDark);
    plot.setPatternOptions(patternBoth, bothStart, bothEnd, evaluateZero, nullptr, false);
    plot.draw(plotView, ctx, rect);
  }
  {
    CurveDrawing plot(evaluate, m_test, nullptr, singleCurveStart, singleCurveEnd, plotView->pixelWidth(), Palette::YellowDark);
    plot.setPatternOptions(patternSingle, singleStart, singleEnd, evaluateZero, nullptr, false);
    plot.draw(plotView, ctx, rect);
  }
}

// TestCurveView

TestCurveView::TestCurveView(Test * test) :
  PlotView(test)
{
  // TestPlotPolicy
  m_test = test;
}

}
