#ifndef STATISTICS_MULTIPLE_HISTOGRAMS_VIEW_H
#define STATISTICS_MULTIPLE_HISTOGRAMS_VIEW_H

#include "../store.h"
#include "histogram_banner_view.h"
#include "histogram_parameter_controller.h"
#include "histogram_view.h"
#include "multiple_data_view.h"

namespace Statistics {

class MultipleHistogramsView : public MultipleDataView {
 public:
  MultipleHistogramsView(Store* store, Shared::CurveViewRange* curveViewRange);
  // MultipleDataView
  HistogramBannerView* bannerView() override { return &m_bannerView; }
  HistogramView* plotViewForSeries(int series) override;

 private:
  void changeDataViewSeriesSelection(int series, bool select) override;

  /* TODO: it would be nice to use an std::array<HistogramView,
   * Store::k_numberOfSeries> here. However HistogramView (and the View parent
   * object) have their default constructor and their move assignment operator
   * deleted, so there is no easy way to achieve that. */
  HistogramView m_histogramView1;
  HistogramView m_histogramView2;
  HistogramView m_histogramView3;
  HistogramView m_histogramView4;
  HistogramView m_histogramView5;
  HistogramView m_histogramView6;

  static constexpr size_t k_numberOfHistogramViews = 6;
  static_assert(k_numberOfHistogramViews == Store::k_numberOfSeries);

  HistogramBannerView m_bannerView;
};

}  // namespace Statistics

#endif
