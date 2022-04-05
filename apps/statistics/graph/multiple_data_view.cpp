#include "multiple_data_view.h"
#include <assert.h>

using namespace Shared;
using namespace Escher;

namespace Statistics {

void MultipleDataView::reload() {
  layoutSubviews();
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    dataViewAtIndex(i)->reload();
  }
}

void MultipleDataView::selectDataView(int index) {
  changeDataViewSelection(index, true);
}

void MultipleDataView::deselectDataView(int index) {
  changeDataViewSelection(index, false);
}

int MultipleDataView::numberOfSubviews() const {
  int result = m_store->numberOfValidSeries();
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
    if (m_store->seriesIsValid(seriesIndex)) {
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

void MultipleDataView::layoutDataSubviews(bool force) {
  int numberDataSubviews = m_store->numberOfValidSeries();
  assert(numberDataSubviews > 0);
  KDCoordinate bannerHeight = bannerView()->minimalSizeForOptimalDisplay().height();
  KDCoordinate subviewHeight = (bounds().height() - bannerHeight)/numberDataSubviews + 1; // +1 to make sure that all pixel rows are drawn
  int displayedSubviewIndex = 0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (m_store->seriesIsValid(i)) {
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

}
