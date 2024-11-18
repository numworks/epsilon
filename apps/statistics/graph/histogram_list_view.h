#ifndef STATISTICS_HISTOGRAM_LIST_VIEW_H
#define STATISTICS_HISTOGRAM_LIST_VIEW_H

#include <escher/selectable_list_view.h>

#include <array>

#include "../store.h"
#include "histogram_banner_view.h"
#include "histogram_view.h"

namespace Statistics {

class HistogramListView : public Escher::SelectableListView {
 public:
  HistogramListView(Store* store, Shared::CurveViewRange* histogramRange,
                    Escher::Responder* parentResponder,
                    Escher::ListViewDataSource* dataSource,
                    Escher::SelectableListViewDataSource* selectionDataSource,
                    Escher::SelectableListViewDelegate* delegate);

 private:
  std::array<HistogramView, Store::k_numberOfSeries> m_histogramViews;

  Store* m_store;
};

}  // namespace Statistics

#endif
