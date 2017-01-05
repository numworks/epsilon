#include "histogram_view.h"
#include <assert.h>
#include <math.h>

namespace Statistics {

HistogramView::HistogramView(Store * store, BannerView * bannerView) :
  CurveViewWithBanner(store, bannerView, 0.2f, 0.1f, 0.4f, 0.1f),
  m_store(store)
{
}

void HistogramView::reloadSelection() {
  float pixelLowerBound = floatToPixel(Axis::Horizontal, m_store->selectedBar() - m_store->barWidth())-1;
  float pixelUpperBound = floatToPixel(Axis::Horizontal, m_store->selectedBar() + m_store->barWidth())+1;
  float selectedValueInPixels = floatToPixel(Axis::Vertical, (float)m_store->heightForBarAtValue(m_store->selectedBar())/m_store->sumOfColumn(1))-1;
  float horizontalAxisInPixels = floatToPixel(Axis::Vertical,  0.0f)+1;
  KDRect dirtyZone(KDRect(pixelLowerBound, selectedValueInPixels, pixelUpperBound-pixelLowerBound,
    horizontalAxisInPixels - selectedValueInPixels));
  markRectAsDirty(dirtyZone);
}

void HistogramView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawLabels(ctx, rect, Axis::Horizontal, false);
  if (m_mainViewSelected) {
    drawHistogram(ctx, rect, nullptr, m_store->firsBarAbscissa(), m_store->barWidth(), true, KDColorBlack, KDColorRed,
      m_store->selectedBar() - m_store->barWidth()/2, m_store->selectedBar() + m_store->barWidth()/2);
  } else {
    drawHistogram(ctx, rect, nullptr, m_store->firsBarAbscissa(), m_store->barWidth(), true, KDColorBlack, KDColorRed);
  }
}

char * HistogramView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

float HistogramView::evaluateModelWithParameter(Model * curve, float t) const {
  return (float)m_store->heightForBarAtValue(t)/m_store->sumOfColumn(1);
}

}
