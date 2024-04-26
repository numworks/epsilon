#include "distribution_curve_view.h"

#include <poincare/old/coordinate_2D.h>

#include "../models/distribution/normal_distribution.h"

using namespace Escher;
using namespace Shared;

namespace Distributions {

// DistributionPlotPolicy

template <typename T>
static T evaluateDistribution1D(T x, void *model, void *) {
  Distribution *distribution = reinterpret_cast<Distribution *>(model);
  return distribution->evaluateAtAbscissa(x);
}

static Poincare::Coordinate2D<float> evaluateDistribution2D(float x,
                                                            void *model,
                                                            void *context) {
  return Poincare::Coordinate2D<float>(
      x, evaluateDistribution1D<float>(x, model, context));
}

static Poincare::Coordinate2D<float> evaluateZero(float, void *, void *) {
  return Poincare::Coordinate2D<float>(0.f, 0.f);
}

static bool barIsHighlighted(double x, void *, void *context) {
  double *parameters = reinterpret_cast<double *>(context);
  double start = parameters[0];
  double end = parameters[1];
  return start <= x && x <= end;
}

void DistributionPlotPolicy::drawPlot(const Shared::AbstractPlotView *plotView,
                                      KDContext *ctx, KDRect rect) const {
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

DistributionCurveView::DistributionCurveView(Distribution *distribution,
                                             Calculation *calculation)
    : PlotView(distribution) {
  // DistributionPlotPolicy
  m_distribution = distribution;
  m_calculation = calculation;
}

}  // namespace Distributions
