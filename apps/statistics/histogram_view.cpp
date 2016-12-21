#include "histogram_view.h"
#include <assert.h>
#include <float.h>

namespace Statistics {

HistogramView::HistogramView(Data * data) :
  CurveView(data),
  m_data(data),
  m_selectedBins(true),
  m_bannerView(BannerView(data))
{
}

void HistogramView::reload() {
  // TODO: optimize dirtiness
  markRectAsDirty(bounds());
  computeLabels(Axis::Horizontal);
  m_bannerView.reload();
}

bool HistogramView::selectedBins() {
  return m_selectedBins;
}

void HistogramView::selectBins(bool selectedBins) {
  if (m_selectedBins != selectedBins) {
    m_selectedBins = selectedBins;
    markRectAsDirty(bounds());
    layoutSubviews();
  }
}

void HistogramView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(Axis::Horizontal, ctx, rect);
  drawLabels(Axis::Horizontal, true, ctx, rect);
  if (m_selectedBins) {
    drawHistogram(m_data->binWidth(), KDColorBlack, ctx, rect, KDColorRed, m_data->selectedBin());
  } else {
    drawHistogram(m_data->binWidth(), KDColorBlack, ctx, rect, KDColorRed, FLT_MAX);
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

float HistogramView::evaluateCurveAtAbscissa(void * curve, float t) const {
  return (float)m_data->sizeAtValue(t)/(float)m_data->totalSize();
}

}
