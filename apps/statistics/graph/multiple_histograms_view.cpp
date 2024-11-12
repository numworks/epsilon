#include "multiple_histograms_view.h"

#include <assert.h>

namespace Statistics {

MultipleHistogramsView::MultipleHistogramsView(
    Store* store, Shared::CurveViewRange* curveViewRange)
    : MultipleDataView(store),
      m_histogramView1(store, 0, curveViewRange),
      m_histogramView2(store, 1, curveViewRange),
      m_histogramView3(store, 2, curveViewRange),
      m_histogramView4(store, 3, curveViewRange),
      m_histogramView5(store, 4, curveViewRange),
      m_histogramView6(store, 5, curveViewRange) {
  for (size_t i = 0; i < k_numberOfHistogramViews; i++) {
    HistogramView* histView = MultipleHistogramsView::plotViewForSeries(i);
    histView->setDisplayLabels(false);
  }
}

HistogramView* MultipleHistogramsView::plotViewForSeries(int series) {
  assert(series >= 0 && series < k_numberOfHistogramViews);
  HistogramView* views[] = {&m_histogramView1, &m_histogramView2,
                            &m_histogramView3, &m_histogramView4,
                            &m_histogramView5, &m_histogramView6};
  return views[series];
}

void MultipleHistogramsView::changeDataViewSeriesSelection(int series,
                                                           bool select) {
  MultipleDataView::changeDataViewSeriesSelection(series, select);
  plotViewForSeries(series)->setDisplayLabels(select);
  if (select == false) {
    // Set the hightlight to default selected bar to prevent blinking
    plotViewForSeries(series)->setHighlight(
        m_store->startOfBarAtIndex(series, k_defaultSelectedIndex),
        m_store->endOfBarAtIndex(series, k_defaultSelectedIndex));
  }
}

}  // namespace Statistics
