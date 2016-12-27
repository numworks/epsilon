#include "histogram_view.h"
#include <assert.h>
#include <math.h>

namespace Statistics {

HistogramView::HistogramView(Data * data) :
  CurveView(data, 0.2f, 0.1f, 0.4f, 0.1f),
  m_data(data),
  m_selectedBins(true),
  m_bannerView(BannerView(data))
{
}

void HistogramView::reload(float dirtyZoneCenter) {
  if (isnan(dirtyZoneCenter)) {
    markRectAsDirty(bounds());
    computeLabels(Axis::Horizontal);
  } else {
    float pixelLowerBound = floatToPixel(Axis::Horizontal, dirtyZoneCenter - m_data->barWidth())-1;
    float pixelUpperBound = floatToPixel(Axis::Horizontal, dirtyZoneCenter + m_data->barWidth())+1;
    float selectedValueInPixels = floatToPixel(Axis::Vertical, (float)m_data->heightForBarAtValue(dirtyZoneCenter)/(float)m_data->totalSize())-1;
    float horizontalAxisInPixels = floatToPixel(Axis::Vertical,  0.0f)+1;
    KDRect dirtyZone(KDRect(pixelLowerBound, selectedValueInPixels, pixelUpperBound-pixelLowerBound,
      horizontalAxisInPixels - selectedValueInPixels));
    markRectAsDirty(dirtyZone);
  }
  m_bannerView.reload();
}

bool HistogramView::selectedBins() {
  return m_selectedBins;
}

void HistogramView::selectBins(bool selectedBins) {
  if (m_selectedBins != selectedBins) {
    reload(m_data->selectedBar());
    m_selectedBins = selectedBins;
    reload(m_data->selectedBar());
    layoutSubviews();
  }
}

void HistogramView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawLabels(ctx, rect, Axis::Horizontal, true);
  if (m_selectedBins) {
    drawHistogram(ctx, rect, nullptr, m_data->firsBarAbscissa(), m_data->barWidth(), true, KDColorBlack, KDColorRed,
      m_data->selectedBar() - m_data->barWidth()/2, m_data->selectedBar() + m_data->barWidth()/2);
  } else {
    drawHistogram(ctx, rect, nullptr, m_data->firsBarAbscissa(), m_data->barWidth(), true, KDColorBlack, KDColorRed);
  }
}

int HistogramView::numberOfSubviews() const {
  return 1;
};

View * HistogramView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_bannerView;
}

void HistogramView::layoutSubviews() {
  KDRect bannerFrame(KDRect(0, bounds().height()- k_bannerHeight, bounds().width(), k_bannerHeight));
  if (!m_selectedBins) {
    bannerFrame = KDRectZero;
  }
  m_bannerView.setFrame(bannerFrame);
}

char * HistogramView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

float HistogramView::evaluateModelWithParameter(Model * curve, float t) const {
  return (float)m_data->heightForBarAtValue(t)/(float)m_data->totalSize();
}

}
