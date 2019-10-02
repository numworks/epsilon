#include "multiple_boxes_view.h"
#include <assert.h>

using namespace Shared;

namespace Statistics {

MultipleBoxesView::MultipleBoxesView(Store * store, BoxView::Quantile * selectedQuantile) :
  MultipleDataView(store),
  m_boxView1(store, 0, nullptr, selectedQuantile, DoublePairStore::colorOfSeriesAtIndex(0), DoublePairStore::colorLightOfSeriesAtIndex(0)),
  m_boxView2(store, 1, nullptr, selectedQuantile, DoublePairStore::colorOfSeriesAtIndex(1), DoublePairStore::colorLightOfSeriesAtIndex(1)),
  m_boxView3(store, 2, nullptr, selectedQuantile, DoublePairStore::colorOfSeriesAtIndex(2), DoublePairStore::colorLightOfSeriesAtIndex(2)),
  m_axisView(store),
  m_bannerView()
{
}

BoxView *  MultipleBoxesView::dataViewAtIndex(int index) {
  assert(index >= 0 && index < 3);
  BoxView * views[] = {&m_boxView1, &m_boxView2, &m_boxView3};
  return views[index];
}

int MultipleBoxesView::seriesOfSubviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews() - 1);
  return static_cast<BoxView *>(subviewAtIndex(index))->series();
}

void MultipleBoxesView::layoutDataSubviews(bool force) {
  int numberOfDataSubviews = m_store->numberOfNonEmptySeries();
  assert(numberOfDataSubviews > 0);
  KDCoordinate bannerHeight = bannerFrame().height();
  KDCoordinate subviewHeight = (bounds().height() - bannerHeight - k_axisViewHeight)/numberOfDataSubviews;
  int displayedSubviewIndex = 0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (!m_store->seriesIsEmpty(i)) {
      KDRect frame = KDRect(0, displayedSubviewIndex*subviewHeight, bounds().width(), subviewHeight);
      dataViewAtIndex(i)->setFrame(frame, force);
      displayedSubviewIndex++;
    }
  }
  m_axisView.setFrame(KDRect(0, displayedSubviewIndex*subviewHeight, bounds().width(), bounds().height() - bannerHeight - displayedSubviewIndex*subviewHeight), force);
}

void MultipleBoxesView::reload() {
  MultipleDataView::reload();
  m_axisView.reload();
}

int MultipleBoxesView::numberOfSubviews() const {
  return MultipleDataView::numberOfSubviews() + 1; // +1 for the axis view
}

View *  MultipleBoxesView::subviewAtIndex(int index) {
  if (index == numberOfSubviews() - 1) {
    return &m_axisView;
  }
  return MultipleDataView::subviewAtIndex(index);
}


}
