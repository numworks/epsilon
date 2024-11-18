#include "histogram_list_view.h"

namespace Statistics {

HistogramListView::HistogramListView(
    Store* store, Shared::CurveViewRange* histogramRange,
    Escher::Responder* parentResponder, Escher::ListViewDataSource* dataSource,
    Escher::SelectableListViewDataSource* selectionDataSource,
    Escher::SelectableListViewDelegate* delegate)
    : Escher::SelectableListView(parentResponder, dataSource,
                                 selectionDataSource, delegate),
      m_histogramViews{HistogramView(store, 0, histogramRange),
                       HistogramView(store, 1, histogramRange),
                       HistogramView(store, 2, histogramRange),
                       HistogramView(store, 3, histogramRange),
                       HistogramView(store, 4, histogramRange),
                       HistogramView(store, 5, histogramRange)},
      m_store(store) {
  for (HistogramView& histogramView : m_histogramViews) {
    histogramView.setDisplayLabels(false);
  }
}

}  // namespace Statistics
