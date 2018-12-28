#include "law_curve_view.h"
#include "law/normal_law.h"
#include <assert.h>

using namespace Shared;

namespace Probability {

void LawCurveView::reload() {
  CurveView::reload();
  markRectAsDirty(bounds());
}

void LawCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  float lowerBound = m_calculation->lowerBound();
  float upperBound = m_calculation->upperBound();
  ctx->fillRect(bounds(), Palette::WallScreen);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawLabels(ctx, rect, Axis::Horizontal, false);
  if (m_law->type() == Law::Type::Normal) {
    // Special case for the normal law, which has always the same curve
    float pixelColorLowerBound = std::round(floatToPixel(Axis::Horizontal, lowerBound));
    float pixelColorUpperBound = std::round(floatToPixel(Axis::Horizontal, upperBound));
    drawStandardNormal(ctx, rect, pixelColorLowerBound, pixelColorUpperBound);
    return;
  }
  if (m_law->isContinuous()) {
    drawCurve(ctx, rect, EvaluateAtAbscissa, m_law, nullptr, Palette::YellowDark, true, lowerBound, upperBound, true);
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
  CurveViewRange * previousRange = constCastedThis->curveViewRange();

  // Draw a centered reduced normal curve
  NormalLaw n;
  constCastedThis->setCurveViewRange(&n);
  drawCurve(ctx, rect, EvaluateAtAbscissa, &n, nullptr, Palette::YellowDark, true, pixelToFloat(Axis::Horizontal, colorLowerBound), pixelToFloat(Axis::Horizontal, colorUpperBound), true);

  // Put back the previous curve view range
  constCastedThis->setCurveViewRange(previousRange);
}

}
