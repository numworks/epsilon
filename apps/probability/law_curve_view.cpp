#include "law_curve_view.h"
#include <assert.h>
#include <math.h>

using namespace Shared;

namespace Probability {

LawCurveView::LawCurveView(Law * law) :
  CurveView(law, nullptr, nullptr, nullptr),
  m_labels{},
  m_law(law),
  m_calculation(nullptr)
{
  assert(law != nullptr);
}

void LawCurveView::setCalculation(Calculation * calculation) {
  m_calculation = calculation;
}

void LawCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  float lowerBound = m_calculation->lowerBound();
  float upperBound = m_calculation->upperBound();
  ctx->fillRect(bounds(), Palette::WallScreen);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawLabels(ctx, rect, Axis::Horizontal, false);
  if (m_law->isContinuous()) {
    drawCurve(ctx, rect, m_law, Palette::YellowDark, true, lowerBound, upperBound, true);
  } else {
    drawHistogram(ctx, rect, m_law, 0, 1, false, Palette::GreyMiddle, Palette::YellowDark, lowerBound, upperBound);
  }
}

char * LawCurveView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

float LawCurveView::evaluateModelWithParameter(void * law, float abscissa) const {
  return m_law->evaluateAtAbscissa(abscissa);
}

}
