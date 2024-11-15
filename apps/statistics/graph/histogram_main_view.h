#ifndef STATISTICS_HISTOGRAM_MAIN_VIEW_H
#define STATISTICS_HISTOGRAM_MAIN_VIEW_H

#include <escher/selectable_list_view.h>

#include <array>

#include "../store.h"
#include "histogram_banner_view.h"
#include "histogram_view.h"

namespace Statistics {

class HistogramMainView : public Escher::SelectableListView {
 public:
  HistogramMainView(
      Store* store, Shared::CurveViewRange* histogramRange,
      Responder* parentResponder, Escher::ListViewDataSource* dataSource,
      Escher::SelectableListViewDataSource* selectionDataSource = nullptr,
      Escher::SelectableListViewDelegate* delegate = nullptr);

  // HistogramBannerView* bannerView() { return &m_bannerView; }

 private:
  std::array<HistogramView, Store::k_numberOfSeries> m_histogramViews;
  // HistogramBannerView m_bannerView;

  Store* m_store;
};

}  // namespace Statistics

#endif
