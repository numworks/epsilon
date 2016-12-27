#include "law_curve_view.h"
#include <assert.h>
#include <math.h>

namespace Probability {

LawCurveView::LawCurveView() :
  CurveView(),
  m_law(nullptr),
  m_calculation(nullptr)
{
}

void LawCurveView::setLaw(Law * law) {
  setCurveViewWindow(law);
  m_law = law;
}

void LawCurveView::setCalculation(Calculation * calculation) {
  m_calculation = calculation;
}

void LawCurveView::reload() {
  markRectAsDirty(bounds());
  computeLabels(Axis::Horizontal);
}

void LawCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  float lowerBound = m_calculation->lowerBound();
  float upperBound = m_calculation->upperBound();
  ctx->fillRect(bounds(), KDColorWhite);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawLabels(ctx, rect, Axis::Horizontal, false);
  if (m_law->isContinuous()) {
    drawCurve(ctx, rect, m_law, KDColorRed, true, lowerBound, upperBound, true);
  } else {
    drawHistogram(ctx, rect, m_law, 0, 1, false, KDColorBlue, KDColorRed, lowerBound, upperBound+1);
  }
}

char * LawCurveView::label(Axis axis, int index) const {
  assert(axis == Axis::Horizontal);
  return (char *)m_labels[index];
}

float LawCurveView::evaluateModelWithParameter(void * law, float abscissa) const {
  return m_law->evaluateAtAbscissa(abscissa);
}

}
