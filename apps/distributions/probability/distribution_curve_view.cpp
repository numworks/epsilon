#include "distribution_curve_view.h"

#include "distributions/models/distribution/normal_distribution.h"

using namespace Shared;
using namespace Escher;

namespace Distributions {

constexpr KDColor DistributionCurveView::k_backgroundColor;

void DistributionCurveView::reload(bool resetInterrupted, bool force) {
  CurveView::reload(resetInterrupted, force);
  markRectAsDirty(bounds());
}

void DistributionCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  float lowerBound = m_calculation->lowerBound();
  float upperBound = m_calculation->upperBound();
  ctx->fillRect(bounds(), k_backgroundColor);
  drawAxis(ctx, rect, Axis::Horizontal);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, false, false, 0, k_backgroundColor);
  if (m_distribution->type() == Poincare::Distribution::Type::Normal) {
    /* Special case for the normal distribution, which has always the same curve
     * We indicate the pixels from and to which we color under the curve, not
     * the float values, as we change the curve parameters. */
    float pixelColorLowerBound = std::round(floatToPixel(Axis::Horizontal, lowerBound));
    float pixelColorUpperBound = std::round(floatToPixel(Axis::Horizontal, upperBound));
    drawStandardNormal(ctx, rect, pixelColorLowerBound, pixelColorUpperBound);
    return;
  }
  if (m_distribution->isContinuous()) {
    drawCartesianCurve(ctx, rect, -INFINITY, INFINITY, EvaluateXYAtAbscissa, m_distribution, nullptr,
                       Palette::YellowDark, CurveView::NoPotentialDiscontinuity, true, true, Palette::YellowDark, lowerBound, upperBound);
  } else {
    drawHistogram(ctx, rect, EvaluateAtAbscissa, m_distribution, nullptr, 0, 1, false, Palette::GrayMiddle,
                  Palette::YellowDark, 0, KDColorBlack, lowerBound, upperBound + 0.5f);
  }
}

char * DistributionCurveView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

float DistributionCurveView::EvaluateAtAbscissa(float abscissa, void * model, void * context) {
  Distribution * distribution = (Distribution *)model;
  return distribution->evaluateAtAbscissa(abscissa);
}

Poincare::Coordinate2D<float> DistributionCurveView::EvaluateXYAtAbscissa(float abscissa, void * model,
                                                                          void * context) {
  return Poincare::Coordinate2D<float>(abscissa, EvaluateAtAbscissa(abscissa, model, context));
}

void DistributionCurveView::drawStandardNormal(KDContext * ctx, KDRect rect, float colorLowerBoundPixel,
                                               float colorUpperBoundPixel) const {
  // Save the previous curve view range
  DistributionCurveView * constCastedThis = const_cast<DistributionCurveView *>(this);
  CurveViewRange * previousRange = curveViewRange();

  // Draw a centered reduced normal curve
  NormalDistribution n;
  constCastedThis->setCurveViewRange(&n);
  drawCartesianCurve(ctx, rect, -INFINITY, INFINITY, EvaluateXYAtAbscissa, &n, nullptr, Palette::YellowDark, CurveView::NoPotentialDiscontinuity, true, true, Palette::YellowDark,
                     pixelToFloat(Axis::Horizontal, colorLowerBoundPixel),
                     pixelToFloat(Axis::Horizontal, colorUpperBoundPixel));

  // Put back the previous curve view range
  constCastedThis->setCurveViewRange(previousRange);
}

}  // namespace Distributions
