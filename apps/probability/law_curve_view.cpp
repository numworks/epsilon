#include "law_curve_view.h"
#include <assert.h>

using namespace Shared;

namespace Probability {

LawCurveView::LawCurveView(Law * law, Calculation * calculation) :
  CurveView(law, nullptr, nullptr, nullptr),
  m_labels{},
  m_law(law),
  m_calculation(calculation)
{
  assert(law != nullptr);
  assert(calculation != nullptr);
}

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

}
