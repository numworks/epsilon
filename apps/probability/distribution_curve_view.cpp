#include "distribution_curve_view.h"
#include "distribution/normal_distribution.h"
#include <assert.h>

using namespace Shared;

namespace Probability {

constexpr KDColor DistributionCurveView::k_backgroundColor;

void DistributionCurveView::reload() {
  CurveView::reload();
  markRectAsDirty(bounds());
}

void DistributionCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  float lowerBound = m_calculation->lowerBound();
  float upperBound = m_calculation->upperBound();
  ctx->fillRect(bounds(), k_backgroundColor);
  drawAxis(ctx, rect, Axis::Horizontal);
  drawLabels(ctx, rect, Axis::Horizontal, false, false, false, 0, k_backgroundColor);
  if (m_distribution->type() == Distribution::Type::Normal) {
    // Special case for the normal distribution, which has always the same curve
    drawStandardNormal(ctx, rect, lowerBound, upperBound);
    return;
  }
  if (m_distribution->isContinuous()) {
    drawCartesianCurve(ctx, rect, EvaluateAtAbscissa, m_distribution, nullptr, Palette::YellowDark, true, lowerBound, upperBound);
  } else {
    drawHistogram(ctx, rect, EvaluateAtAbscissa, m_distribution, nullptr, 0, 1, false, Palette::GreyMiddle, Palette::YellowDark, lowerBound, upperBound+0.5f);
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

void DistributionCurveView::drawStandardNormal(KDContext * ctx, KDRect rect, float colorLowerBound, float colorUpperBound) const {
  // Save the previous curve view range
  DistributionCurveView * constCastedThis = const_cast<DistributionCurveView *>(this);
  CurveViewRange * previousRange = curveViewRange();

  // Draw a centered reduced normal curve
  NormalDistribution n;
  constCastedThis->setCurveViewRange(&n);
  drawCartesianCurve(ctx, rect, EvaluateAtAbscissa, &n, nullptr, Palette::YellowDark, true, colorLowerBound, colorUpperBound);

  // Put back the previous curve view range
  constCastedThis->setCurveViewRange(previousRange);
}

}
