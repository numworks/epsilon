#include "multiple_data_view.h"
#include <assert.h>

using namespace Shared;

namespace Statistics {

void MultipleDataView::setDisplayBanner(bool display) {
  m_displayBanner = display;
  layoutBanner(false);
}

void MultipleDataView::reload() {
  layoutSubviews();
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    dataViewAtIndex(i)->reload();
  }
}

int MultipleDataView::indexOfSubviewAtSeries(int series) {
  int displayedSubviewIndex = 0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (!m_store->seriesIsEmpty(i)) {
     if (i == series) {
        return displayedSubviewIndex;
     }
     displayedSubviewIndex++;
    } else if (i == series) {
      return -1;
    }
  }
  assert(false);
  return -1;
}

void MultipleDataView::selectDataView(int index) {
  changeDataViewSelection(index, true);
}

void MultipleDataView::deselectDataView(int index) {
  changeDataViewSelection(index, false);
}

int MultipleDataView::numberOfSubviews() const {
  int result = m_store->numberOfNonEmptySeries();
  assert(result <= Store::k_numberOfSeries);
  return result + 1; // +1 for the banner view
}

View * MultipleDataView::subviewAtIndex(int index) {
  if (index == MultipleDataView::numberOfSubviews() -1) {
    return bannerView();
  }
  int seriesIndex = 0;
  int nonEmptySeriesIndex = -1;
  while (seriesIndex < Store::k_numberOfSeries) {
    if (!m_store->seriesIsEmpty(seriesIndex)) {
      nonEmptySeriesIndex++;
      if (nonEmptySeriesIndex == index) {
        return dataViewAtIndex(seriesIndex);
      }
    }
    seriesIndex++;
  }
  assert(false);
  return nullptr;
}

void MultipleDataView::layoutSubviews(bool force) {
  // We need to set the banner width first, so its height can be computed
  bannerView()->setFrame(KDRect(0, 0, bounds().width(), 0), force);
  layoutDataSubviews(force);
  layoutBanner(force);
}

void MultipleDataView::layoutDataSubviews(bool force) {
  int numberDataSubviews = m_store->numberOfNonEmptySeries();
  assert(numberDataSubviews > 0);
  KDCoordinate bannerHeight = bannerView()->minimalSizeForOptimalDisplay().height();
  KDCoordinate subviewHeight = (bounds().height() - bannerHeight)/numberDataSubviews + 1; // +1 to make sure that all pixel rows are drawn
  int displayedSubviewIndex = 0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (!m_store->seriesIsEmpty(i)) {
      CurveView * dataView = dataViewAtIndex(i);
      KDRect frame = KDRect(0, displayedSubviewIndex*subviewHeight, bounds().width(), subviewHeight);
      dataView->setFrame(frame, force);
      displayedSubviewIndex++;
    }
  }
}

void MultipleDataView::changeDataViewSelection(int index, bool select) {
  dataViewAtIndex(index)->selectMainView(select);
  dataViewAtIndex(index)->reload();
}

KDRect MultipleDataView::bannerFrame() const {
  KDCoordinate bannerHeight = const_cast<MultipleDataView *>(this)->bannerView()->minimalSizeForOptimalDisplay().height();
  KDRect frame = KDRect(0, bounds().height() - bannerHeight, bounds().width(), bannerHeight);
  return frame;
}

void MultipleDataView::layoutBanner(bool force) {
  KDCoordinate bannerHeight = bannerView()->minimalSizeForOptimalDisplay().height();
  if (m_displayBanner) {
    bannerView()->setFrame(bannerFrame(), force);
  } else {
    KDRect frame = KDRect(0, bounds().height() - bannerHeight, bounds().width(), 0);
    bannerView()->setFrame(frame, force);
  }
}

void MultipleDataView::drawRect(KDContext * ctx, KDRect rect) const {
  if (!m_displayBanner) {
    ctx->fillRect(bannerFrame(), KDColorWhite);
  }
}

}
