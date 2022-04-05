#include "data_view.h"

namespace Statistics {

void DataView::selectViewForSeries(int series) {
  changeDataViewSeriesSelection(series, true);
}

void DataView::deselectViewForSeries(int series) {
  changeDataViewSeriesSelection(series, false);
}

void DataView::setDisplayBanner(bool display) {
  m_displayBanner = display;
  layoutBanner(false);
}

void DataView::changeDataViewSeriesSelection(int series, bool select) {
  curveViewForSeries(series)->selectMainView(select);
  curveViewForSeries(series)->reload();
}

KDRect DataView::bannerFrame() const {
  KDCoordinate bannerHeight = const_cast<DataView *>(this)->bannerView()->minimalSizeForOptimalDisplay().height();
  KDRect frame = KDRect(0, bounds().height() - bannerHeight, bounds().width(), bannerHeight);
  return frame;
}

void DataView::drawRect(KDContext * ctx, KDRect rect) const {
  if (!m_displayBanner) {
    ctx->fillRect(bannerFrame(), KDColorWhite);
  }
}

void DataView::layoutSubviews(bool force) {
  // We need to set the banner width first, so its height can be computed
  bannerView()->setFrame(KDRect(0, 0, bounds().width(), 0), force);
  layoutDataSubviews(force);
  layoutBanner(force);
}


void DataView::layoutBanner(bool force) {
  KDCoordinate bannerHeight = bannerView()->minimalSizeForOptimalDisplay().height();
  if (m_displayBanner) {
    bannerView()->setFrame(bannerFrame(), force);
  } else {
    KDRect frame = KDRect(0, bounds().height() - bannerHeight, bounds().width(), 0);
    bannerView()->setFrame(frame, force);
  }
}

}
