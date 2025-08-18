#include "distribution_curve_view.h"

#include <poincare/coordinate_2D.h>

#include "../models/distribution/normal_distribution.h"

using namespace Escher;
using namespace Shared;

namespace Distributions {

// DistributionPlotPolicy

template <typename T>
static T evaluateDistribution1D(T x, const void* model, const void*) {
  const Distribution* distribution = static_cast<const Distribution*>(model);
  return distribution->evaluateAtAbscissa(x);
}

static Poincare::Coordinate2D<float> evaluateDistribution2D(
    float x, const void* model, const void* context) {
  return Poincare::Coordinate2D<float>(
      x, evaluateDistribution1D<float>(x, model, context));
}

static Poincare::Coordinate2D<float> evaluateZero(float, const void*,
                                                  const void*) {
  return Poincare::Coordinate2D<float>(0.f, 0.f);
}

static bool barIsHighlighted(double x, const void*, const void* context) {
  const double* parameters = static_cast<const double*>(context);
  double start = parameters[0];
  double end = parameters[1];
  return start <= x && x <= end;
}

void DistributionPlotPolicy::drawPlot(const Shared::AbstractPlotView* plotView,
                                      KDContext* ctx, KDRect rect) const {
  double lowerBound = m_calculation->lowerBound();
  double upperBound = m_calculation->upperBound();

  if (m_distribution->isContinuous()) {
    CurveDrawing plot(Curve2D(evaluateDistribution2D, m_distribution), nullptr,
                      m_distribution->xMin(), m_distribution->xMax(),
                      plotView->pixelWidth(), Palette::YellowDark, true);
    plot.setPatternOptions(Pattern(Palette::YellowDark), lowerBound, upperBound,
                           Curve2D(evaluateZero), Curve2D(), false);
    plot.draw(plotView, ctx, rect);
  } else {
    double context[] = {lowerBound, upperBound};
    HistogramDrawing plot(evaluateDistribution1D<double>, m_distribution,
                          context, barIsHighlighted, 0.f, 1.f, false, false,
                          Palette::GrayMiddle, Palette::YellowDark);
    plot.draw(plotView, ctx, rect);
  }
}

// DistributionCurveView

DistributionCurveView::DistributionCurveView(Distribution* distribution,
                                             Calculation* calculation)
    : PlotView(distribution) {
  // DistributionPlotPolicy
  m_distribution = distribution;
  m_calculation = calculation;
}

}  // namespace Distributions
