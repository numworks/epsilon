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

void MultipleBoxesView::layoutSubviews() {
  MultipleDataView::layoutSubviews();
  int numberOfDataSubviews = numberOfSubviews() - 1;
  assert(numberOfDataSubviews > 0);
  for (int i = 0; i < numberOfDataSubviews; i++) {
    static_cast<BoxView *>(subviewAtIndex(i))->setDisplayAxis(i == numberOfDataSubviews - 1);
  }
}

}
