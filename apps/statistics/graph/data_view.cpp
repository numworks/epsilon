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
  plotViewForSeries(series)->setFocus(select);
  plotViewForSeries(series)->reload();
}

KDRect DataView::bannerFrame() const {
  KDCoordinate bannerHeight = const_cast<DataView *>(this)
                                  ->bannerView()
                                  ->minimalSizeForOptimalDisplay()
                                  .height();
  return KDRect(0, bounds().height() - bannerHeight, bounds().width(),
                bannerHeight);
}

void DataView::layoutSubviews(bool force) {
  // We need to set the banner width first, so its height can be computed
  bannerView()->setFrame(KDRect(0, 0, bounds().width(), 0), force);
  layoutDataSubviews(force);
  layoutBanner(force);
}

void DataView::layoutBanner(bool force) {
  bannerView()->setFrame(m_displayBanner ? bannerFrame() : KDRectZero, force);
}

}  // namespace Statistics
