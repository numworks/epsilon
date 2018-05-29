#include "multiple_histograms_view.h"
#include <assert.h>

using namespace Shared;

namespace Statistics {

MultipleHistogramsView::MultipleHistogramsView(HistogramController * controller, Store * store) :
  MultipleDataView(store),
  m_histogramView1(controller, store, 0, nullptr, Palette::Red),
  m_histogramView2(controller, store, 1, nullptr, Palette::Blue),
  m_histogramView3(controller, store, 2, nullptr, Palette::Green),
  // TODO Share colors with stats/store_controller
  m_bannerView(),
  m_okView()
{
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    HistogramView * histView = dataViewAtIndex(i);
    histView->setDisplayBannerView(false);
    histView->setDisplayLabels(false);
    histView->setForceOkDisplay(true);
  }
}

HistogramView *  MultipleHistogramsView::dataViewAtIndex(int index) {
  assert(index >= 0 && index < 3);
  HistogramView * views[] = {&m_histogramView1, &m_histogramView2, &m_histogramView3};
  return views[index];
}

int MultipleHistogramsView::seriesOfSubviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews() - 1);
  return static_cast<HistogramView *>(subviewAtIndex(index))->series();
}

void MultipleHistogramsView::layoutSubviews() {
  MultipleDataView::layoutSubviews();
  int numberHistogramSubviews = m_store->numberOfNonEmptySeries();
  assert(numberHistogramSubviews > 0);
  int displayedSubviewIndex = 0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (!m_store->seriesIsEmpty(i)) {
      dataViewAtIndex(i)->setOkView(displayedSubviewIndex == numberHistogramSubviews - 1 ? &m_okView : nullptr);
      displayedSubviewIndex++;
    }
  }
}

void MultipleHistogramsView::changeDataViewSelection(int index, bool select) {
  MultipleDataView::changeDataViewSelection(index, select);
  dataViewAtIndex(index)->setDisplayLabels(select);
}

}
