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
  m_bannerView()
{
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    BoxView * boxView = dataViewAtIndex(i);
    boxView->setDisplayBannerView(false);
    boxView->setDisplayAxis(false);
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
  KDCoordinate axisHeight = 30;
  KDCoordinate subviewHeight = (bounds().height() - bannerHeight - axisHeight)/numberOfDataSubviews;
  int displayedSubviewIndex = 0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (!m_store->seriesIsEmpty(i)) {
      BoxView * boxView = dataViewAtIndex(i);
      bool displaysAxis = displayedSubviewIndex == numberOfDataSubviews - 1;
      boxView->setDisplayAxis(displaysAxis);
      KDCoordinate currentSubviewHeight = subviewHeight + (displaysAxis ? axisHeight : 0);
      KDRect frame = KDRect(0, displayedSubviewIndex*subviewHeight, bounds().width(), currentSubviewHeight);
      boxView->setFrame(frame);
      displayedSubviewIndex++;
    }
  }
}

}
