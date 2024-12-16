#include "multiple_histograms_view.h"

#include <assert.h>

namespace Statistics {

MultipleHistogramsView::MultipleHistogramsView(
    Store* store, Shared::CurveViewRange* curveViewRange)
    : MultipleDataView(store),
      m_histogramViews{HistogramView(store, 0, curveViewRange),
                       HistogramView(store, 1, curveViewRange),
                       HistogramView(store, 2, curveViewRange),
                       HistogramView(store, 3, curveViewRange),
                       HistogramView(store, 4, curveViewRange),
                       HistogramView(store, 5, curveViewRange)} {
  for (HistogramView& histogramView : m_histogramViews) {
    histogramView.setDisplayLabels(false);
  }
}

HistogramView* MultipleHistogramsView::plotViewForSeries(int series) {
  assert(series >= 0 && series < m_histogramViews.size());
  return &m_histogramViews[series];
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
