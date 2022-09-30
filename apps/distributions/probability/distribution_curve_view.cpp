#include "distribution_curve_view.h"
#include "../models/distribution/normal_distribution.h"

using namespace Escher;
using namespace Poincare;
using namespace Shared;

namespace Distributions {

// DistributionPlotPolicy

static float evaluateDistribution1D(float x, void * model, void *) {
  Distribution * distribution = reinterpret_cast<Distribution *>(model);
  return distribution->evaluateAtAbscissa(x);
}

static Coordinate2D<float> evaluateDistribution2D(float x, void * model, void * context) {
  return Coordinate2D<float>(x, evaluateDistribution1D(x, model, context));
}

static Coordinate2D<float> evaluateZero(float, void *, void *) { return Coordinate2D<float>(0.f, 0.f); }

static bool barIsHighlighted(float x, void *, void * context) {
  float * parameters = reinterpret_cast<float *>(context);
  float start = parameters[0];
  float end = parameters[1];
  return start <= x && x <= end;
}

void DistributionPlotPolicy::drawPlot(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  float lowerBound = m_calculation->lowerBound();
  float upperBound = m_calculation->upperBound();

  if (m_distribution->type() == Poincare::Distribution::Type::Normal) {
    /* Special case for the normal distribution, which has always the same curve
     * We indicate the pixels from and to which we color under the curve, not
     * the float values, as we change the curve parameters. */
    float pixelColorLowerBound = std::round(plotView->floatToPixel(AbstractPlotView::Axis::Horizontal, lowerBound));
    float pixelColorUpperBound = std::round(plotView->floatToPixel(AbstractPlotView::Axis::Horizontal, upperBound));
    drawStandardNormal(plotView, ctx, rect, pixelColorLowerBound, pixelColorUpperBound);
  } else if (m_distribution->isContinuous()) {
    CurveDrawing plot(evaluateDistribution2D, m_distribution, nullptr, m_distribution->xMin(), m_distribution->xMax(), plotView->pixelWidth(), Palette::YellowDark, true);
    plot.setPatternOptions(Pattern(Palette::YellowDark), lowerBound, upperBound, evaluateZero, nullptr, nullptr, nullptr, false);
    plot.draw(plotView, ctx, rect);
  } else {
    float context[] = { lowerBound, upperBound };
    HistogramDrawing plot(evaluateDistribution1D, m_distribution, context, 0.f, 1.f, Palette::GrayMiddle, false);
    plot.setHighlightOptions(barIsHighlighted, Palette::YellowDark);
    plot.draw(plotView, ctx, rect);
  }
}

void DistributionPlotPolicy::drawStandardNormal(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float colorLowerBound, float colorUpperBound) const {
  // Save the previous curve view range
  CurveViewRange * previousRange = plotView->range();

  // Draw a centered reduced normal curve
  AbstractPlotView * mutablePlotView = const_cast<AbstractPlotView *>(plotView);
  NormalDistribution n;
  mutablePlotView->setRange(&n);

  float colorStart = mutablePlotView->pixelToFloat(AbstractPlotView::Axis::Horizontal, colorLowerBound);
  float colorEnd = mutablePlotView->pixelToFloat(AbstractPlotView::Axis::Horizontal, colorUpperBound);

  CurveDrawing plot(evaluateDistribution2D, &n, nullptr, n.xMin(), n.xMax(), mutablePlotView->pixelWidth(), Palette::YellowDark, true);
  plot.setPatternOptions(Pattern(Palette::YellowDark), colorStart, colorEnd, evaluateZero, nullptr, nullptr, nullptr, false);
  plot.draw(mutablePlotView, ctx, rect);

  // Put back the previous curve view range
  mutablePlotView->setRange(previousRange);
}

// DistributionCurveView

DistributionCurveView::DistributionCurveView(Distribution * distribution, Calculation * calculation) :
  PlotView(distribution)
{
  // DistributionPlotPolicy
  m_distribution = distribution;
  m_calculation = calculation;
}

void DistributionCurveView::reload(bool resetInterruption, bool force) {
  AbstractPlotView::reload(resetInterruption, force);
  markRectAsDirty(bounds());
}

}
