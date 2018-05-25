#include "multiple_boxes_view.h"
#include <assert.h>

using namespace Shared;

namespace Statistics {

MultipleBoxesView::MultipleBoxesView(Store * store, BoxView::Quantile * selectedQuantile) :
  MultipleDataView(store),
  m_boxView1(store, 0, nullptr, selectedQuantile, Palette::Red),
  m_boxView2(store, 1, nullptr, selectedQuantile, Palette::Blue),
  m_boxView3(store, 2, nullptr, selectedQuantile, Palette::Green),
  // TODO Share colors with stats/store_controller
  m_axisView(store),
  m_bannerView()
{
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    BoxView * boxView = dataViewAtIndex(i);
    boxView->setDisplayBannerView(false);
  }
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

void MultipleBoxesView::layoutDataSubviews() {
  int numberOfDataSubviews = m_store->numberOfNonEmptySeries();
  assert(numberOfDataSubviews > 0);
  KDCoordinate bannerHeight = bannerFrame().height();
  KDCoordinate subviewHeight = (bounds().height() - bannerHeight - k_axisViewHeight)/numberOfDataSubviews;
  int displayedSubviewIndex = 0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (!m_store->seriesIsEmpty(i)) {
      KDRect frame = KDRect(0, displayedSubviewIndex*subviewHeight, bounds().width(), subviewHeight);
      dataViewAtIndex(i)->setFrame(frame);
      displayedSubviewIndex++;
    }
  }
  m_axisView.setFrame(KDRect(0, displayedSubviewIndex*subviewHeight, bounds().width(), k_axisViewHeight));
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
