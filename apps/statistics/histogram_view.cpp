#include "histogram_view.h"
#include <assert.h>
#include <math.h>

namespace Statistics {

HistogramView::HistogramView(Data * data) :
  CurveViewWithBanner(data, 0.2f, 0.1f, 0.4f, 0.1f),
  m_data(data),
  m_bannerView(HistogramBannerView(data))
{
}

void HistogramView::reloadSelection() {
  float pixelLowerBound = floatToPixel(Axis::Horizontal, m_data->selectedBar() - m_data->barWidth())-1;
  float pixelUpperBound = floatToPixel(Axis::Horizontal, m_data->selectedBar() + m_data->barWidth())+1;
  float selectedValueInPixels = floatToPixel(Axis::Vertical, (float)m_data->heightForBarAtValue(m_data->selectedBar())/(float)m_data->totalSize())-1;
  float horizontalAxisInPixels = floatToPixel(Axis::Vertical,  0.0f)+1;
  KDRect dirtyZone(KDRect(pixelLowerBound, selectedValueInPixels, pixelUpperBound-pixelLowerBound,
    horizontalAxisInPixels - selectedValueInPixels));
  markRectAsDirty(dirtyZone);
  m_bannerView.reload();
}

void HistogramView::reload() {
  markRectAsDirty(bounds());
  computeLabels(Axis::Horizontal);
  m_bannerView.reload();
}

void HistogramView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawLabels(ctx, rect, Axis::Horizontal, false);
  if (m_mainViewSelected) {
    drawHistogram(ctx, rect, nullptr, m_data->firsBarAbscissa(), m_data->barWidth(), true, KDColorBlack, KDColorRed,
      m_data->selectedBar() - m_data->barWidth()/2, m_data->selectedBar() + m_data->barWidth()/2);
  } else {
    drawHistogram(ctx, rect, nullptr, m_data->firsBarAbscissa(), m_data->barWidth(), true, KDColorBlack, KDColorRed);
  }
}

char * HistogramView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

View * HistogramView::bannerView() {
  return &m_bannerView;
}

float HistogramView::evaluateModelWithParameter(Model * curve, float t) const {
  return (float)m_data->heightForBarAtValue(t)/(float)m_data->totalSize();
}

}
