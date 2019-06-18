#include "law_curve_view.h"
#include "law/normal_law.h"
#include <assert.h>

using namespace Shared;

namespace Probability {

constexpr KDColor LawCurveView::k_backgroundColor;

void LawCurveView::reload() {
  CurveView::reload();
  markRectAsDirty(bounds());
}

void LawCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  float lowerBound = m_calculation->lowerBound();
  float upperBound = m_calculation->upperBound();
  ctx->fillRect(bounds(), k_backgroundColor);
  drawAxis(ctx, rect, Axis::Horizontal);
  drawLabels(ctx, rect, Axis::Horizontal, false, false, false, 0, k_backgroundColor);
  if (m_law->type() == Law::Type::Normal) {
    // Special case for the normal law, which has always the same curve
    drawStandardNormal(ctx, rect, lowerBound, upperBound);
    return;
  }
  if (m_law->isContinuous()) {
    drawCurve(ctx, rect, EvaluateAtAbscissa, m_law, nullptr, Palette::YellowDark, true, lowerBound, upperBound);
  } else {
    drawHistogram(ctx, rect, EvaluateAtAbscissa, m_law, nullptr, 0, 1, false, Palette::GreyMiddle, Palette::YellowDark, lowerBound, upperBound+0.5f);
  }
}

char * LawCurveView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

float LawCurveView::EvaluateAtAbscissa(float abscissa, void * model, void * context) {
  Law * law = (Law *)model;
  return law->evaluateAtAbscissa(abscissa);
}

void LawCurveView::drawStandardNormal(KDContext * ctx, KDRect rect, float colorLowerBound, float colorUpperBound) const {
  // Save the previous curve view range
  LawCurveView * constCastedThis = const_cast<LawCurveView *>(this);
  CurveViewRange * previousRange = curveViewRange();

  // Draw a centered reduced normal curve
  NormalLaw n;
  constCastedThis->setCurveViewRange(&n);
  drawCurve(ctx, rect, EvaluateAtAbscissa, &n, nullptr, Palette::YellowDark, true, colorLowerBound, colorUpperBound);

  // Put back the previous curve view range
  constCastedThis->setCurveViewRange(previousRange);
}

}
