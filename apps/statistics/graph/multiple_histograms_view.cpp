#include "multiple_histograms_view.h"
#include <assert.h>

using namespace Shared;

namespace Statistics {

MultipleHistogramsView::MultipleHistogramsView(HistogramController * controller, Store * store) :
  MultipleDataView(store),
  m_histogramView1(controller, store, 0, nullptr, DoublePairStore::colorOfSeriesAtIndex(0)),
  m_histogramView2(controller, store, 1, nullptr, DoublePairStore::colorOfSeriesAtIndex(1)),
  m_histogramView3(controller, store, 2, nullptr, DoublePairStore::colorOfSeriesAtIndex(2))
{
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    HistogramView * histView = dataViewAtIndex(i);
    histView->setDisplayLabels(false);
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

void MultipleHistogramsView::layoutSubviews(bool force) {
  MultipleDataView::layoutSubviews();
  assert(m_store->numberOfValidSeries() > 0);
  int displayedSubviewIndex = 0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (m_store->seriesIsValid(i)) {
      displayedSubviewIndex++;
    }
  }
}

void MultipleHistogramsView::changeDataViewSelection(int index, bool select) {
  MultipleDataView::changeDataViewSelection(index, select);
  dataViewAtIndex(index)->setDisplayLabels(select);
  if (select == false) {
    // Set the hightlight to default selected bar to prevent blinking
    dataViewAtIndex(index)->setHighlight(m_store->startOfBarAtIndex(index, k_defaultSelectedBar), m_store->endOfBarAtIndex(index, k_defaultSelectedBar));
  }
}

}
